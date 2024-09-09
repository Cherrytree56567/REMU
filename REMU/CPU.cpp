#include "CPU.h"

CPU::CPU(std::shared_ptr<Bus> BUS) {
    for (auto& csra : CSRegisters) {
        csra = 0;
    }
    for (auto& regs : Registers) {
        regs = 0;
    }
    Registers[0] = 0x00; // Register x0 hardwired to 0
    Registers[2] = MEMORY_BASE + MEMORY_SIZE; // Set Stack Pointer
    ProgramCounter = MEMORY_BASE;
    bus = BUS;
    Memory = bus->GetRam();
    page_table = 0;
    enable_paging = false;
    cycle_counter = 0;
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
        pte = HandleException(bus->Load(a + vpn[i] * 8, 64));

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
            if ((csrRead(MSTATUS) >> 3) & 1 == 0) {
                return None;
            }
        }; break;
        case Mode::Supervisor: {
            if ((csrRead(SSTATUS) >> 1) & 1 == 0) {
                return None;
            }
        }; break;
        default: break;
    }

    uint32_t irq;

    if (bus->GetUart()->is_interrupting()) {
        irq = UART_IRQ;
    } else if (bus->GetVirtio()->is_interrupting()) {
        // Access disk by direct dram access (DMA). An interrupt is raised after a disk
        // access is done.
        Virtio{ {} }.disk_access(this);
        irq = VIRTIO_IRQ;
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
    p_pc = HandleException(pl);
    if (p_pc == 0) {
		return Exception::InstructionAccessFault;
	}
    return MemoryLoad(p_pc, 32);
}

bool CPU::Loop() {
    uint64_t inst;
    auto result = Fetch();
    if (std::holds_alternative<uint64_t>(result)) {
        inst = std::get<uint64_t>(result);
    }
    else {
        ExceptionTrap ext(std::get<Exception>(result));
        ext.take_trap(std::any_cast<std::shared_ptr<CPU>>(cp));
        if (ext.is_fatal(std::get<Exception>(result))) {
            debug(Warning, "Exception - {}", static_cast<int>(ext.exception_code()));
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
            debug(Warning, "Exception - {}", static_cast<int>(ext.exception_code()));
        }
    }

    Interrupt sads = check_pending_interrupt();
    if (sads != Interrupt::None) {
        InterruptTrap sssss(sads);
        sssss.take_trap(std::any_cast<std::shared_ptr<CPU>>(cp));
    }

    return true;
}

std::variant<uint64_t, Exception> CPU::MemoryLoad(uint64_t addr, uint64_t size) {
    uint64_t p_addr;
    auto pl = translate(addr, AccessType::Load);
    p_addr = HandleException(pl);
    return bus->Load(p_addr, size);
}

std::variant<uint64_t, Exception> CPU::MemoryStore(uint64_t addr, uint64_t size, uint64_t value) {
    uint64_t p_addr;
    auto pl = translate(addr, AccessType::Store);
    p_addr = HandleException(pl);
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

uint64_t CPU::HandleException(std::variant<uint64_t, Exception> s) {
    if (std::holds_alternative<uint64_t>(s)) {
        return std::get<uint64_t>(s);
    } else {
        if (EnableDebug) {
            std::cout << "Exception: " << (int)std::get<Exception>(s) << std::endl;
        }
        return 0;
    }
}

uint64_t CPU::csrRead(uint64_t csr) {
    switch (csr) {
    case SIE: return CSRegisters[MIE] & CSRegisters[MIDELEG]; break;
    default: return CSRegisters[csr]; break;
    }
}

void CPU::csrWrite(uint64_t csr, uint64_t value) {
    switch (csr) {
    case SIE: CSRegisters[MIE] = (CSRegisters[MIE] & !CSRegisters[MIDELEG]) | (value & CSRegisters[MIDELEG]); break;
    default: CSRegisters[csr] = value; break;
    }
}

std::variant<uint64_t, Exception> CPU::Execute(uint64_t inst) {
    DumpRegisters();
    using namespace std::chrono;
    int opcode = inst & 0x0000007f;     // opcode in bits 6..0
    int funct3 = (inst & 0x00007000) >> 12;    // funct3 in bits 14..12
    int funct7 = (inst & 0xfe000000) >> 25;   // funct7 in bits 31..25
    int funct5 = (funct7 & 0b1111100) >> 2;

    Registers[0] = 0;
    cycle_counter += 1;

    // Add your code for measuring elapsed time and calculating MHz
    auto current_time = steady_clock::now();
    double time_elapsed = duration_cast<duration<double>>(current_time - start_time).count();

    switch (opcode) {
    case 0x37:   exec_LUI(inst); break;  // RV32I Base
    case 0x17: exec_AUIPC(inst); break;  // RV32I Base

    case 0x6f:   exec_JAL(inst); break;  // RV32I Base
    case 0x67:  exec_JALR(inst); break;  // RV32I Base

    case 0x63: // B-Type
        switch (funct3) {
        case 0x0:   exec_BEQ(inst); break;  // RV32I Base
        case 0x1:   exec_BNE(inst); break;  // RV32I Base
        case 0x4:   exec_BLT(inst); break;  // RV32I Base
        case 0x5:   exec_BGE(inst); break;  // RV32I Base
        case 0x6:  exec_BLTU(inst); break;  // RV32I Base
        case 0x7:  exec_BGEU(inst); break;  // RV32I Base
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    case 0x03: // Load
        switch (funct3) {
        case 0x0:   exec_LB(inst); break;  // RV32I Base
        case 0x1:   exec_LH(inst); break;  // RV32I Base 
        case 0x2:   exec_LW(inst); break;  // RV32I Base  
        case 0x3:   exec_LD(inst); break;  // RV64I Base
        case 0x4:  exec_LBU(inst); break;  // RV32I Base 
        case 0x5:  exec_LHU(inst); break;  // RV32I Base 
        case 0x6:  exec_LWU(inst); break;  // RV64I Base
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    case 0x23: // S-Type
        switch (funct3) {
        case 0x0:  exec_SB(inst); break;  // RV32I Base  
        case 0x1:  exec_SH(inst); break;  // RV32I Base  
        case 0x2:  exec_SW(inst); break;  // RV32I Base  
        case 0x3:  exec_SD(inst); break;  // RV64I Base
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    case 0x13: // I-Type
        switch (funct3) {
        case 0x0:   exec_ADDI(inst); break;  // RV32I Base
        case 0x1:   exec_SLLI(inst); break;  // RV64I Base
        case 0x2:   exec_SLTI(inst); break;  // RV32I Base
        case 0x3:  exec_SLTIU(inst); break;  // RV32I Base
        case 0x4:   exec_XORI(inst); break;  // RV32I Base
        case 0x5: // SRI
            switch (funct7 >> 1) {
            case 0x00:  exec_SRLI(inst); break;  // RV64I Base
            case 0x10:  exec_SRAI(inst); break;  // RV64I Base
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, Funct7: {:#x}, Funct7 >> 1: {:#x}", opcode, funct3, funct7, funct7 >> 1);
                return Exception::IllegalInstruction;
            } break;
            } break;
        case 0x6:   exec_ORI(inst); break;  // RV32I Base
        case 0x7:  exec_ANDI(inst); break;  // RV32I Base
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    case 0x33: // R-Type
        switch (funct3) {
        case 0x00:
            switch (funct7) {
            case 0x00: exec_ADD(inst); break; // RV32I Base
            case 0x01: exec_MUL(inst); break; // RV32I Base
            case 0x20: exec_SUB(inst); break; // RV32I Base
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, Funct7: {:#x}", opcode, funct3, funct7);
                return Exception::IllegalInstruction;
            } break;
            } break;
        case 0x1:   exec_SLL(inst); break; // RV32I Base
        case 0x2:   exec_SLT(inst); break; // RV32I Base
        case 0x3:  exec_SLTU(inst); break; // RV32I Base
        case 0x4:   exec_XOR(inst); break; // RV32I Base
        case 0x5:
            switch (funct7) {
            case 0x00:  exec_SRL(inst); break; // RV32I Base
            case 0x20:  exec_SRA(inst); break; // RV32I Base
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, Funct7: {:#x}", opcode, funct3, funct7);
                return Exception::IllegalInstruction;
            } break;
            }
        case 0x6:   exec_OR(inst); break; // RV32I Base
        case 0x7:  exec_AND(inst); break; // RV32I Base
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    case 0x0f: exec_FENCE(inst); break;   // RV32I Base

    case 0x1b: // I-Type 64
        switch (funct3) {
        case 0x0: exec_ADDIW(inst); break; // RV64I Base
        case 0x1: exec_SLLIW(inst); break; // RV64I Base
        case 0x5:
            switch (funct7) {
            case 0x00: exec_SRLIW(inst); break; // RV64I Base
            case 0x20: exec_SRAIW(inst); break; // RV64I Base
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, Funct7: {:#x}", opcode, funct3, funct7);
                return Exception::IllegalInstruction;
            } break;
            } break;
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    case 0x3b: // R-Type 64
        switch (funct3) {
        case 0x0:
            switch (funct7) {
            case 0x00:  exec_ADDW(inst); break; // RV64I Base
            case 0x20:  exec_SUBW(inst); break; // RV64I Base
            case 0x01:  exec_MULW(inst); break; // RV64M Standard
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, Funct7: {:#x}", opcode, funct3, funct7);
                return Exception::IllegalInstruction;
            } break;
            } break;
        case 0x4:  exec_DIVW(inst); break; // RV64M Standard
        case 0x1:  exec_SLLW(inst); break; // RV64I Base
        case 0x5:
            switch (funct7) {
            case 0x00:   exec_SRLW(inst); break; // RV64I Base
            case 0x20:   exec_SRAW(inst); break; // RV64I Base
            case 0x01:  exec_DIVUW(inst); break; // RV64M Standard
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, Funct7: {:#x}", opcode, funct3, funct7);
                return Exception::IllegalInstruction;
            } break;
            } break;
        case 0x6:  exec_REMW(inst); break; // RV64M Standard
        case 0x7: exec_REMUW(inst); break; // RV64M Standard
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    case 0x73:
        switch (funct3) {
        case 0x0: {
            switch (rs2(inst)) {
            case 0x0: exec_ECALL(inst); break;   // RV32I Base
            case 0x1: exec_EBREAK(inst); break;  // RV32I Base
            case 0x2: switch (funct7) {    // RV64I Base
                case 0x8: exec_SRET(inst); break;  // RV32I Base
                case 0x18: exec_MRET(inst); break;  // RV32I Base
                default: {
                    debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, rs2: {:#x}, funct7: {:#x}", opcode, funct3, rs2(inst), funct7);
                    return Exception::IllegalInstruction;
                } break;
            } break;
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, rs2: {:#x}", opcode, funct3, rs2(inst));
                return Exception::IllegalInstruction;
            } break;
            }
        } break; // RV32I Base
        case 0x1:  exec_CSRRW(inst); break; // RV64I Base
        case 0x2:  exec_CSRRS(inst); break; // RV64I Base
        case 0x3:  exec_CSRRC(inst); break; // RV64I Base
        case 0x5:  exec_CSRRWI(inst); break; // RV64I Base
        case 0x6:  exec_CSRRSI(inst); break; // RV64I Base
        case 0x7:  exec_CSRRCI(inst); break; // RV64I Base
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    case 0x2f:
        switch (funct3) { // since, funct[1:0] = aq, rl
        case 0x2: switch (funct5) {
            case 0x00: exec_AMOADD_W(inst); break; // RV32A Standard
            case 0x01: exec_AMOSWAP_W(inst); break; // RV32A Standard
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, Funct5: {:#x}", opcode, funct3, funct5);
                return Exception::IllegalInstruction;
            } break;
        } break;
        case 0x3: switch (funct5) {
            case 0x00: exec_AMOADD_D(inst); break; // RV32A Standard
            case 0x01: exec_AMOSWAP_D(inst); break; // RV32A Standard
            default: {
                debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}, Funct5: {:#x}", opcode, funct3, funct5);
                return Exception::IllegalInstruction;
            } break;
        } break;
        default: {
            debug(Warning, "Instruction Not Supported: Opcode: {:#x}, Funct3: {:#x}", opcode, funct3);
            return Exception::IllegalInstruction;
        } break;
        } break;

    default: {
        debug(Warning, "Instruction Not Supported: Opcode: {:#x}", opcode);
        return Exception::IllegalInstruction;
    } break;
    }
    return (uint64_t)0;
}