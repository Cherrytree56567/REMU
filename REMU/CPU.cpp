#include "CPU.h"

CPU::CPU(std::shared_ptr<Bus> BUS) {
    Registers[0] = 0x00; // Register x0 hardwired to 0
    Registers[2] = MEMORY_BASE + MEMORY_SIZE; // Set Stack Pointer
    ProgramCounter = MEMORY_BASE;
    bus = BUS;
    Memory = bus->GetRam();
    for (auto& csra : CSRegisters) {
        csra = 0;
    }
    page_table = 0;
    enable_paging = false;
}

void CPU::adcp(std::any cpp) {
    cp = cpp;
}

void CPU::update_paging(size_t csr_addr) {
    if (csr_addr != SATP) {
        return;
    }

    page_table = (csrRead(SATP) & ((1 << 44) - 1)) * PAGE_SIZE;

    auto mode = csrRead(SATP) >> 60;

    if (mode == 8) {
        enable_paging = true;
    } else {
        enable_paging = false;
    }
}

std::variant<uint64_t, Exception> CPU::translate(uint64_t addr, AccessType access_type) {
    if (!enable_paging) {
        return addr;
    }

    int levels = 3;

    uint64_t vpn[3] = {
        (addr >> 12) & 0x1ff,
        (addr >> 21) & 0x1ff,
        (addr >> 30) & 0x1ff
    };

    uint64_t a = page_table;

    int64_t i = levels - 1;

    uint64_t pte;

    while (true) {
        auto po = bus->Load(a + vpn[i] * 8, 64);
        if (std::holds_alternative<uint64_t>(po)) {
            pte = std::get<uint64_t>(po);
        }
        else {
            pte = 0;
                std::cout << "Failed to bus load.\n";
        }

        uint64_t v = pte & 1;

        uint64_t r = (pte >> 1) & 1;

        uint64_t w = (pte >> 2) & 1;

        uint64_t x = (pte >> 3) & 1;

        if (v == 0 || (r == 0 && w == 1)) {
            switch (access_type) {
            case AccessType::Instruction: return Exception::InstructionPageFault; break;
            case AccessType::Load: return Exception::LoadPageFault; break;
            case AccessType::Store: return Exception::StoreAMOPageFault; break;
            }
        }

        if (r == 1 || x == 1) {
            break;
        }

        i -= 1;

        uint64_t ppn = (pte >> 10) & 0x0fffffffffff;

        a = ppn * PAGE_SIZE;

        if (i < 0) {
            switch (access_type) {
            case AccessType::Instruction: return Exception::InstructionPageFault; break;
            case AccessType::Load: return Exception::LoadPageFault; break;
            case AccessType::Store: return Exception::StoreAMOPageFault; break;
            }
        }

    }

    uint64_t ppn[3] = {
        (pte >> 10) & 0x1ff,
        (pte >> 19) & 0x1ff,
        (pte >> 28) & 0x03ffffff
    };

    uint64_t offset = addr & 0xfff;

    switch (i) {
    case 0: {
        uint64_t ppn = (pte >> 10) & 0x0fffffffffff;
        return (ppn << 12) | offset;
    } break;
    case 1: {
        return (ppn[2] << 30) | (ppn[1] << 21) | (vpn[0] << 12) | offset;
    } break;
    case 2: {
        return (ppn[2] << 30) | (vpn[1] << 21) | (vpn[0] << 12) | offset;
    }
    default: switch (access_type) {
    case AccessType::Instruction: return Exception::InstructionPageFault; break;
    case AccessType::Load: return Exception::LoadPageFault; break;
    case AccessType::Store: return Exception::StoreAMOPageFault; break;
    } break;
    }
}

Interrupt CPU::check_pending_interrupt() {
    switch (CurrentMode) {
        case Mode::Machine: {
            if ((csrRead(MSTATUS) >> 3) & (1 == 0)) {
                return None;
            }
        }; break;
        case Mode::Supervisor: {
            if ((csrRead(SSTATUS) >> 1) & (1 == 0)) {
                return None;
            }
        }; break;
        default: break;
    }

    uint32_t irq;

    if (bus->GetUart()->is_interrupting()) {
        irq = UART_IRQ;
    } else {
        irq = 0;
    }

    if (irq != 0) {
        if (std::holds_alternative<Exception>(bus->Store(PLIC_SCLAIM, 32, irq))) {
            std::cout << "Failed to write an IRQ to the PLIC_SCLAIM\n";
        }
        csrWrite(MIP, csrRead(MIP) | MIP_SEIP);
    }

    uint32_t pending = csrRead(MIE) & csrRead(MIP);

    if ((pending & MIP_MEIP) != 0) {
        csrWrite(MIP, csrRead(MIP) & !MIP_MEIP);
        return Interrupt::MachineExternalInterrupt;
    }

    if ((pending & MIP_MSIP) != 0) {
        csrWrite(MIP, csrRead(MIP) & !MIP_MSIP);
        return Interrupt::MachineSoftwareInterrupt;
    }

    if ((pending & MIP_MTIP) != 0) {
        csrWrite(MIP, csrRead(MIP) & !MIP_MTIP);
        return Interrupt::MachineTimerInterrupt;
    }

    if ((pending & MIP_SEIP) != 0) {
        csrWrite(MIP, csrRead(MIP) & !MIP_SEIP);
        return Interrupt::SupervisorExternalInterrupt;
    }

    if ((pending & MIP_SSIP) != 0) {
        csrWrite(MIP, csrRead(MIP) & !MIP_SSIP);
        return Interrupt::SupervisorSoftwareInterrupt;
    }

    if ((pending & MIP_STIP) != 0) {
        csrWrite(MIP, csrRead(MIP) & !MIP_STIP);
       return Interrupt::SupervisorTimerInterrupt;
    }
    
    return None;
}

std::variant<uint64_t, Exception> CPU::Fetch() {
    uint64_t p_pc;
    auto pl = translate(ProgramCounter, AccessType::Instruction);
    if (std::holds_alternative<uint64_t>(pl)) {
        p_pc = std::get<uint64_t>(pl);
    }
    else {
        p_pc = 0;
        std::cout << "Failed to translate pc.\n";
    }
    return MemoryLoad(p_pc, 32);
}

void CPU::debug(std::string s) {
    if (EnableDebug) {
        std::cout << "[REMU] INFO: " << s;
    }
}

bool CPU::Loop() {
    uint32_t inst;
    auto result = Fetch();
    if (std::holds_alternative<uint64_t>(result)) {
        inst = std::get<uint64_t>(result);
    }
    else {
        ExceptionTrap ext(std::get<Exception>(result));
        ext.take_trap(std::any_cast<std::shared_ptr<CPU>>(cp));
        if (ext.is_fatal(std::get<Exception>(result))) {
            return false;
        }
        inst = 0;
    }

    ProgramCounter += 4;

    auto resulta = Execute(inst);

    if (std::holds_alternative<uint64_t>(resulta)) {
        std::cout << "";
    }
    else {
        ExceptionTrap ext(std::get<Exception>(resulta));
        ext.take_trap(std::any_cast<std::shared_ptr<CPU>>(cp));
        if (ext.is_fatal(std::get<Exception>(resulta))) {
            return false;
        }
    }

    Interrupt sads = check_pending_interrupt();
    if (sads != Interrupt::None) {
        InterruptTrap sssss(sads);
        sssss.take_trap(std::any_cast<std::shared_ptr<CPU>>(cp));
    }

    if (ProgramCounter == 0)
        return false;

    return true;
}

std::variant<uint64_t, Exception> CPU::MemoryLoad(uint64_t addr, uint64_t size) {
    uint64_t p_addr;
    auto pl = translate(addr, AccessType::Load);
    if (std::holds_alternative<uint64_t>(pl)) {
        p_addr = std::get<uint64_t>(pl);
    }
    else {
        p_addr = 0;
        std::cout << "Failed to translate addr.\n";
    }
    return bus->Load(p_addr, size);
}

std::variant<uint64_t, Exception> CPU::MemoryStore(uint64_t addr, uint64_t size, uint64_t value) {
    uint64_t p_addr;
    auto pl = translate(addr, AccessType::Store);
    if (std::holds_alternative<uint64_t>(pl)) {
        p_addr = std::get<uint64_t>(pl);
    }
    else {
        p_addr = 0;
        std::cout << "Failed to translate addr.\n";
    }
    return bus->Store(p_addr, size, value);
}

void CPU::DumpRegisters() {
    const char* abi[] = { // Application Binary Interface registers
        "zero", "ra",  "sp",  "gp",
          "tp", "t0",  "t1",  "t2",
          "s0", "s1",  "a0",  "a1",
          "a2", "a3",  "a4",  "a5",
          "a6", "a7",  "s2",  "s3",
          "s4", "s5",  "s6",  "s7",
          "s8", "s9", "s10", "s11",
          "t3", "t4",  "t5",  "t6"
    };

    /*for (int i=0; i<8; i++) {*/
        /*printf("%4s| x%02d: %#-8.2lx\t", abi[i],    i,    Registers[i]);*/
        /*printf("%4s| x%02d: %#-8.2lx\t", abi[i+8],  i+8,  Registers[i+8]);*/
        /*printf("%4s| x%02d: %#-8.2lx\t", abi[i+16], i+16, Registers[i+16]);*/
        /*printf("%4s| x%02d: %#-8.2lx\n", abi[i+24], i+24, Registers[i+24]);*/
    /*}*/

    if (EnableDebug) {
        for (int i = 0; i < 8; i++) {
            printf("   %4s: %#-13.2lx  ", abi[i], Registers[i]);
            printf("   %2s: %#-13.2lx  ", abi[i + 8], Registers[i + 8]);
            printf("   %2s: %#-13.2lx  ", abi[i + 16], Registers[i + 16]);
            printf("   %3s: %#-13.2lx\n", abi[i + 24], Registers[i + 24]);
        }
    }
}

uint64_t CPU::csrRead(uint64_t csr) {
    return (uint64_t)(uint32_t)CSRegisters[csr];
}

void CPU::csrWrite(uint64_t csr, uint64_t value) {
    CSRegisters[csr] = value;
}

std::variant<uint64_t, Exception> CPU::Execute(uint32_t inst) {
    int opcode = inst & 0x7f;           // opcode in bits 6..0
    int funct3 = (inst >> 12) & 0x7;    // funct3 in bits 14..12
    int funct7 = (inst >> 25) & 0x7f;   // funct7 in bits 31..25

    Registers[0] = 0;

    switch (opcode) {
    case LUI:   exec_LUI(inst); break;  // RV32I Base
    case AUIPC: exec_AUIPC(inst); break;  // RV32I Base

    case JAL:   exec_JAL(inst); break;  // RV32I Base
    case JALR:  exec_JALR(inst); break;  // RV32I Base

    case B_TYPE:
        switch (funct3) {
        case BEQ:   exec_BEQ(inst); break;  // RV32I Base
        case BNE:   exec_BNE(inst); break;  // RV32I Base
        case BLT:   exec_BLT(inst); break;  // RV32I Base
        case BGE:   exec_BGE(inst); break;  // RV32I Base
        case BLTU:  exec_BLTU(inst); break;  // RV32I Base
        case BGEU:  exec_BGEU(inst); break;  // RV32I Base
        default:;
        } break;

    case LOAD:
        switch (funct3) {
        case LB:  exec_LB(inst); break;  // RV32I Base
        case LH:  exec_LH(inst); break;  // RV32I Base 
        case LW:  exec_LW(inst); break;  // RV32I Base  
        case LD:  exec_LD(inst); break;  // RV64I Base
        case LBU:  exec_LBU(inst); break;  // RV32I Base 
        case LHU:  exec_LHU(inst); break;  // RV32I Base 
        case LWU:  exec_LWU(inst); break;  // RV64I Base
        default:;
        } break;

    case S_TYPE:
        switch (funct3) {
        case SB:  exec_SB(inst); break;  // RV32I Base  
        case SH:  exec_SH(inst); break;  // RV32I Base  
        case SW:  exec_SW(inst); break;  // RV32I Base  
        case SD:  exec_SD(inst); break;  // RV64I Base
        default:;
        } break;

    case I_TYPE:
        switch (funct3) {
        case ADDI:  exec_ADDI(inst); break;  // RV32I Base
        case SLLI:  exec_SLLI(inst); break;  // RV64I Base
        case SLTI:  exec_SLTI(inst); break;  // RV32I Base
        case SLTIU: exec_SLTIU(inst); break;  // RV32I Base
        case XORI:  exec_XORI(inst); break;  // RV32I Base
        case SRI:
            switch (funct7) {
            case SRLI:  exec_SRLI(inst); break;  // RV64I Base
            case SRAI:  exec_SRAI(inst); break;  // RV64I Base
            default:;
            } break;
        case ORI:   exec_ORI(inst); break;  // RV32I Base
        case ANDI:  exec_ANDI(inst); break;  // RV32I Base
        default:
            fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
            return Exception::IllegalInstruction;
        } break;

    case R_TYPE:
        switch (funct3) {
        case ADDSUB:
            switch (funct7) {
            case IM: 
                switch ((inst >> 25) & ((1 << 2) - 1)) {
                case ITYPE: exec_ADD(inst); break; // RV32I Base
                case MTYPE: 
                    switch (funct3) {
                    case MUL: exec_MUL(inst); break; // RV32M Standard
                    case MULH: exec_MULH(inst); break; // RV32M Standard
                    case MULHSU: exec_MULHSU(inst); break; // RV32M Standard
                    case MULHU: exec_MULHU(inst); break; // RV32M Standard
                    case DIV: exec_DIV(inst); break; // RV32M Standard
                    case DIVU: exec_DIVU(inst); break; // RV32M Standard
                    case REM: exec_REM(inst); break; // RV32M Standard
                    case REMU: exec_REMU(inst); break; // RV32M Standard
                    } break;
                } break;
            case SUB: exec_ADD(inst); break; // RV32I Base
            default:;
            } break;
        case SLL:  exec_SLL(inst); break; // RV32I Base
        case SLT:  exec_SLT(inst); break; // RV32I Base
        case SLTU: exec_SLTU(inst); break; // RV32I Base
        case XOR:  exec_XOR(inst); break; // RV32I Base
        case SR:
            switch (funct7) {
            case SRL:  exec_SRL(inst); break; // RV32I Base
            case SRA:  exec_SRA(inst); break; // RV32I Base
            default:;
            }
        case OR:   exec_OR(inst); break; // RV32I Base
        case AND:  exec_AND(inst); break; // RV32I Base
        default:
            fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
            return Exception::IllegalInstruction;
        } break;

    case FENCE: exec_FENCE(inst); break;   // RV32I Base

    case I_TYPE_64:
        switch (funct3) {
        case ADDIW: exec_ADDIW(inst); break; // RV64I Base
        case SLLIW: exec_SLLIW(inst); break; // RV64I Base
        case SRIW:
            switch (funct7) {
            case SRLIW: exec_SRLIW(inst); break; // RV64I Base
            case SRAIW: exec_SRAIW(inst); break; // RV64I Base
            } break;
        } break;

    case R_TYPE_64:
        switch (funct3) {
        case ADDSUB:
            switch (funct7) {
            case ADDW:  exec_ADDW(inst); break; // RV64I Base
            case SUBW:  exec_SUBW(inst); break; // RV64I Base
            case MULW:  exec_MULW(inst); break; // RV64M Standard
            } break;
        case DIVW:  exec_DIVW(inst); break; // RV64M Standard
        case SLLW:  exec_SLLW(inst); break; // RV64I Base
        case SRW:
            switch (funct7) {
            case SRLW:  exec_SRLW(inst); break; // RV64I Base
            case SRAW:  exec_SRAW(inst); break; // RV64I Base
            case DIVUW: exec_DIVUW(inst); break; // RV64M Standard
            } break;
        case REMW:  exec_REMW(inst); break; // RV64M Standard
        case REMUW: exec_REMUW(inst); break; // RV64M Standard
        default:;
        } break;

    case CSR:
        switch (funct3) {
        case ECALLBREAK: return exec_ECALLBREAK(inst); break; // RV32I Base
        case CSRRW:  exec_CSRRW(inst); break; // RV64I Base
        case CSRRS:  exec_CSRRS(inst); break; // RV64I Base
        case CSRRC:  exec_CSRRC(inst); break; // RV64I Base
        case CSRRWI:  exec_CSRRWI(inst); break; // RV64I Base
        case CSRRSI:  exec_CSRRSI(inst); break; // RV64I Base
        case CSRRCI:  exec_CSRRCI(inst); break; // RV64I Base
        default:
            fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
            return Exception::IllegalInstruction;
        } break;

    case AMO_W:
        switch (funct7 >> 2) { // since, funct[1:0] = aq, rl
        case LR_W:  exec_LR_W(inst); break; // RV32A Standard
        case SC_W:  exec_SC_W(inst); break; // RV32A Standard
        case AMOSWAP_W:  exec_AMOSWAP_W(inst); break; // RV32A Standard 
        case AMOADD_W:  exec_AMOADD_W(inst); break; // RV32A Standard
        case AMOXOR_W:  exec_AMOXOR_W(inst); break; // RV32A Standard
        case AMOAND_W:  exec_AMOAND_W(inst); break; // RV32A Standard
        case AMOOR_W:  exec_AMOOR_W(inst); break; // RV32A Standard
        case AMOMIN_W:  exec_AMOMIN_W(inst); break; // RV32A Standard
        case AMOMAX_W:  exec_AMOMAX_W(inst); break; // RV32A Standard
        case AMOMINU_W:  exec_AMOMINU_W(inst); break; // RV32A Standard
        case AMOMAXU_W:  exec_AMOMAXU_W(inst); break; // RV32A Standard
        default:
            fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
            return Exception::IllegalInstruction;
        } break;

    case AMO_D:
        switch (funct7 >> 2) { // since, funct[1:0] = aq, rl
        case LR_D:  exec_LR_D(inst); break; // RV64A Standard
        case SC_D:  exec_SC_D(inst); break; // RV64A Standard
        case AMOSWAP_D:  exec_AMOSWAP_D(inst); break; // RV64 Standard 
        case AMOADD_D:  exec_AMOADD_D(inst); break; // RV64A Standard
        case AMOXOR_D:  exec_AMOXOR_D(inst); break; // RV64A Standard
        case AMOAND_D:  exec_AMOAND_D(inst); break; // RV64A Standard
        case AMOOR_D:  exec_AMOOR_D(inst); break; // RV64A Standard
        case AMOMIN_D:  exec_AMOMIN_D(inst); break; // RV64A Standard
        case AMOMAX_D:  exec_AMOMAX_D(inst); break; // RV64A Standard
        case AMOMINU_D:  exec_AMOMINU_D(inst); break; // RV64A Standard
        case AMOMAXU_D:  exec_AMOMAXU_D(inst); break; // RV64A Standard
        default:
            fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
            return Exception::IllegalInstruction;
        } break;

    case 0x00:
        return Exception::IllegalInstruction;

    default:
        fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct3:0x%x\n", opcode, funct3, funct7);
        return Exception::IllegalInstruction;
        /*exit(1);*/
    }
    return (uint64_t)0;
}