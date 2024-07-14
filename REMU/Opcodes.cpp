#include "CPU.h"
#include <boost/multiprecision/cpp_int.hpp>

void CPU::exec_LUI(uint32_t inst) {
    // LUI places upper 20 bits of U-immediate value to rd
    Registers[rd(inst)] = (uint64_t)(int64_t)(int32_t)(inst & 0xfffff000);
    debug("lui\n");
}

void CPU::exec_AUIPC(uint32_t inst) {
    // AUIPC forms a 32-bit offset from the 20 upper bits 
    // of the U-immediate
    uint64_t imm = imm_U(inst);
    Registers[rd(inst)] = ((int64_t)ProgramCounter + (int64_t)imm) - 4;
    debug("auipc\n");
}

uint32_t wrapping_add(uint32_t a, uint64_t b) {
    // Calculate the sum
    uint64_t sum = static_cast<uint64_t>(a) + b;

    // Wrap around if the sum exceeds UINT32_MAX
    return static_cast<uint32_t>(sum & 0xfffff10e);
}

void CPU::exec_JAL(uint32_t inst) {
    uint64_t imm = imm_J(inst);
    Registers[rd(inst)] = ProgramCounter;
    /*debug("JAL-> rd:%ld, pc:%lx\n", rd(inst), ProgramCounter);*/
    ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("jal\n");
    //std::cout << Registers[rd(inst)] << std::endl;
   // std::cout << imm << std::endl;
    //std::cout << ProgramCounter << std::endl;
    //exit(199);
}

void CPU::exec_JALR(uint32_t inst) {
    using namespace boost::multiprecision;
    uint64_t imm = imm_I(inst);
    uint64_t tmp = ProgramCounter;
    Registers[rd(inst)] = tmp;
    if (((cpp_int)Registers[rs1(inst)] + (cpp_int)imm) > (cpp_int)18446744073709551615) {
        cpp_int ssss = (((cpp_int)Registers[rs1(inst)] + (cpp_int)imm));
        ssss = ssss % ((cpp_int)18446744073709551615 + 1);
        ProgramCounter = ssss.convert_to<uint64_t>();
    } else {
        ProgramCounter = (Registers[rs1(inst)] + imm);
    }
    /*debug("NEXT -> %#lx, imm:%#lx\n", ProgramCounter, imm);*/
    debug("jalr\n");
    std::cout << Registers[rs1(inst)] << std::endl << imm << std::endl << ProgramCounter << std::endl << tmp << std::endl;
    exit(99);
}

void CPU::exec_BEQ(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if ((int64_t)Registers[rs1(inst)] == (int64_t)Registers[rs2(inst)])
        ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("beq\n");
}
void CPU::exec_BNE(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if ((int64_t)Registers[rs1(inst)] != (int64_t)Registers[rs2(inst)])
        ProgramCounter = (ProgramCounter + (int64_t)imm - 4);
    debug("bne\n");
}
void CPU::exec_BLT(uint32_t inst) {
    /*debug("Operation: BLT\n");*/
    uint64_t imm = imm_B(inst);
    if ((int64_t)Registers[rs1(inst)] < (int64_t)Registers[rs2(inst)])
        ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("blt\n");
}
void CPU::exec_BGE(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if ((int64_t)Registers[rs1(inst)] >= (int64_t)Registers[rs2(inst)])
        ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("bge\n");
}
void CPU::exec_BLTU(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] < Registers[rs2(inst)])
        ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("bltu\n");
}
void CPU::exec_BGEU(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] >= Registers[rs2(inst)])
        ProgramCounter = (int64_t)ProgramCounter + (int64_t)imm - 4;
    debug("jal\n");
}
void CPU::exec_LB(uint32_t inst) {
    // load 1 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = (int64_t)(int8_t)std::get<uint64_t>(MemoryLoad(addr, 8));
    debug("lb\n");
}
void CPU::exec_LH(uint32_t inst) {
    // load 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = (int64_t)(int16_t)std::get<uint64_t>(MemoryLoad(addr, 16));
    debug("lh\n");
}
void CPU::exec_LW(uint32_t inst) {
    // load 4 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = (int64_t)(int32_t)std::get<uint64_t>(MemoryLoad(addr, 32));
    debug("lw\n");
}
void CPU::exec_LD(uint32_t inst) {
    // load 8 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = (int64_t)std::get<uint64_t>(MemoryLoad(addr, 64));
    debug("ld\n");
}
void CPU::exec_LBU(uint32_t inst) {
    // load unsigned 1 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(addr, 8));
    debug("lbu\n");
}
void CPU::exec_LHU(uint32_t inst) {
    // load unsigned 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(addr, 16));
    debug("lhu\n");
}
void CPU::exec_LWU(uint32_t inst) {
    // load unsigned 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(addr, 32));
    debug("lwu\n");
}
void CPU::exec_SB(uint32_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    MemoryStore(addr, 8, Registers[rs2(inst)]);
    debug("sb\n");
}
void CPU::exec_SH(uint32_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    MemoryStore(addr, 16, Registers[rs2(inst)]);
    debug("sh\n");
}
void CPU::exec_SW(uint32_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    MemoryStore(addr, 32, Registers[rs2(inst)]);
    debug("sw\n");
}
void CPU::exec_SD(uint32_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    MemoryStore(addr, 64, Registers[rs2(inst)]);
    debug("sd\n");
}

void CPU::exec_ADDI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] + (int64_t)imm;
    debug("addi\n");
}

void CPU::exec_SLLI(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] << shamt(inst);
    debug("slli\n");
}

void CPU::exec_SLTI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (Registers[rs1(inst)] < (int64_t)imm) ? 1 : 0;
    debug("slti\n");
}

void CPU::exec_SLTIU(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (Registers[rs1(inst)] < imm) ? 1 : 0;
    debug("sltiu\n");
}

void CPU::exec_XORI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] ^ imm;
    debug("xori\n");
}

void CPU::exec_SRLI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] >> imm;
    debug("srli\n");
}

void CPU::exec_SRAI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (int32_t)Registers[rs1(inst)] >> imm;
    debug("srai\n");
}

void CPU::exec_ORI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] | imm;
    debug("ori\n");
}

void CPU::exec_ANDI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] & imm;
    debug("andi\n");
}

void CPU::exec_ADD(uint32_t inst) {
    Registers[rd(inst)] =
        (uint64_t)((int64_t)Registers[rs1(inst)] + (int64_t)Registers[rs2(inst)]);
    debug("add\n");
}

void CPU::exec_SUB(uint32_t inst) {
    Registers[rd(inst)] =
        (uint64_t)((int64_t)Registers[rs1(inst)] - (int64_t)Registers[rs2(inst)]);
    debug("sub\n");
}

void CPU::exec_SLL(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] << (int64_t)Registers[rs2(inst)];
    debug("sll\n");
}

void CPU::exec_SLT(uint32_t inst) {
    Registers[rd(inst)] = (Registers[rs1(inst)] < (int64_t)Registers[rs2(inst)]) ? 1 : 0;
    debug("slt\n");
}

void CPU::exec_SLTU(uint32_t inst) {
    Registers[rd(inst)] = (Registers[rs1(inst)] < Registers[rs2(inst)]) ? 1 : 0;
    debug("slti\n");
}

void CPU::exec_XOR(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] ^ Registers[rs2(inst)];
    debug("xor\n");
}

void CPU::exec_SRL(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] >> Registers[rs2(inst)];
    debug("srl\n");
}

void CPU::exec_SRA(uint32_t inst) {
    Registers[rd(inst)] = (int32_t)Registers[rs1(inst)] >>
        (int64_t)Registers[rs2(inst)];
    debug("sra\n");
}

void CPU::exec_OR(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] | Registers[rs2(inst)];
    debug("or\n");
}

void CPU::exec_AND(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] & Registers[rs2(inst)];
    debug("and\n");
}

void CPU::exec_FENCE(uint32_t inst) {
    debug("fence\n");
}

std::variant<uint64_t, Exception> CPU::exec_ECALL(uint32_t inst) {
    switch (CurrentMode) {
    case Mode::Machine:
        return Exception::EnvironmentCallFromMMode;
        break;

    case Mode::Supervisor:
        return Exception::EnvironmentCallFromSMode;
        break;

    case Mode::User:
        return Exception::EnvironmentCallFromUMode;
        break;
    }
    return (uint64_t)0;
}
std::variant<uint64_t, Exception> CPU::exec_EBREAK(uint32_t inst) {
    return Exception::Breakpoint;
}

std::variant<uint64_t, Exception> CPU::exec_ECALLBREAK(uint32_t inst) {
    debug("ecallbreak\n");
    int funct7 = (inst >> 25) & 0x7f;
    if (imm_I(inst) == 0x0) {
        return exec_ECALL(inst);
    } else if (imm_I(inst) == 0x1) {
        return exec_EBREAK(inst);
    } else if (rs2(inst) == 0x2) {
        if (funct7 == 0x8) {
            exec_SRET(inst);
        } else if (funct7 == 0x18) {
            exec_MRET(inst);
        }
    } else if (funct7 == 0x9) {
        exec_SFENCE_VMA(inst);
    }
    return (uint64_t)0;
}


void CPU::exec_ADDIW(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] + (int64_t)imm;
    debug("addiw\n");
}

void CPU::exec_SLLIW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] << shamt(inst));
    debug("slliw\n");
}
void CPU::exec_SRLIW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] >> shamt(inst));
    debug("srliw\n");
}
void CPU::exec_SRAIW(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] >> (uint64_t)(int64_t)(int32_t)imm);
    debug("sraiw\n");
}
void CPU::exec_ADDW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        + (int64_t)Registers[rs2(inst)]);
    debug("addw\n");
}
void CPU::exec_MULW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        * (int64_t)Registers[rs2(inst)]);
    debug("mulw\n");
}
void CPU::exec_SUBW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        - (int64_t)Registers[rs2(inst)]);
    debug("subw\n");
}
void CPU::exec_DIVW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        / (int64_t)Registers[rs2(inst)]);
    debug("divw\n");
}
void CPU::exec_SLLW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] << Registers[rs2(inst)]);
    debug("sllw\n");
}
void CPU::exec_SRLW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] >> Registers[rs2(inst)]);
    debug("srlw\n");
}
void CPU::exec_DIVUW(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] / (int64_t)Registers[rs2(inst)];
    debug("divuw\n");
}
void CPU::exec_SRAW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] >> (uint64_t)(int64_t)(int32_t)Registers[rs2(inst)]);
    debug("sraw\n");
}
void CPU::exec_REMW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        % (int64_t)Registers[rs2(inst)]);
    debug("remw\n");
}
void CPU::exec_REMUW(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] % (int64_t)Registers[rs2(inst)];
    debug("remuw\n");
}

void CPU::exec_LR_W(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 32));
    debug("amolr.w\n");
}
void CPU::exec_SC_W(uint32_t inst) {
    MemoryStore(Registers[rs1(inst)], 32, Registers[rs2(inst)]);
    Registers[rd(inst)] = 0;
    debug("amosc.w\n");
}
void CPU::exec_AMOSWAP_W(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, rs2(inst));
    debug("amoswap.w\n");
}
void CPU::exec_AMOADD_W(uint32_t inst) {
    uint32_t tmp = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 32));
    uint32_t res = tmp + (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoadd.w\n");
}
void CPU::exec_AMOXOR_W(uint32_t inst) {
    uint32_t tmp = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 32));
    uint32_t res = tmp ^ (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoxor.w\n");
}
void CPU::exec_AMOAND_W(uint32_t inst) {
    uint32_t tmp = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 32));
    uint32_t res = tmp & (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoand.w\n");
}
void CPU::exec_AMOOR_W(uint32_t inst) {
    uint32_t tmp = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 32));
    uint32_t res = tmp | (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoor.w\n");
}
void CPU::exec_AMOMIN_W(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amomin.w\n");
}
void CPU::exec_AMOMAX_W(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomax.w\n");
}
void CPU::exec_AMOMINU_W(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amominu.w\n");
}
void CPU::exec_AMOMAXU_W(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomaxu.w\n");
}

void CPU::exec_LR_D(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 64));
    debug("amolr.d\n");
}
void CPU::exec_SC_D(uint32_t inst) {
    MemoryStore(Registers[rs1(inst)], 64, Registers[rs2(inst)]);
    Registers[rd(inst)] = 0;
    debug("amosc.d\n");
}
void CPU::exec_AMOSWAP_D(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, rs2(inst));
    debug("amoswap.d\n");
}
void CPU::exec_AMOADD_D(uint32_t inst) {
    uint64_t tmp = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp + (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoadd.d\n");
}
void CPU::exec_AMOXOR_D(uint32_t inst) {
    uint64_t tmp = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp ^ (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoxor.d\n");
}
void CPU::exec_AMOAND_D(uint32_t inst) {
    uint64_t tmp = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp & (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoand.d\n");
}
void CPU::exec_AMOOR_D(uint32_t inst) {
    uint64_t tmp = std::get<uint64_t>(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp | (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoor.d\n");
}
void CPU::exec_AMOMIN_D(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amomin.d\n");
}
void CPU::exec_AMOMAX_D(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomax.d\n");
}
void CPU::exec_AMOMINU_D(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amominu.d\n");
}
void CPU::exec_AMOMAXU_D(uint32_t inst) {
    Registers[rd(inst)] = std::get<uint64_t>(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomaxu.d\n");
}

void CPU::exec_CSRRW(uint32_t inst) {
    Registers[rd(inst)] = csrRead(csr(inst));
    csrWrite(csr(inst), Registers[rs1(inst)]);
    debug("csrrw\n");
}
void CPU::exec_CSRRS(uint32_t inst) {
    csrWrite(csr(inst), CSRegisters[csr(inst)] | Registers[rs1(inst)]);
    debug("csrrs\n");
}
void CPU::exec_CSRRC(uint32_t inst) {
    csrWrite(csr(inst), CSRegisters[csr(inst)] & !(Registers[rs1(inst)]));
    debug("csrrc\n");
}
void CPU::exec_CSRRWI(uint32_t inst) {
    csrWrite(csr(inst), rs1(inst));
    debug("csrrwi\n");
}
void CPU::exec_CSRRSI(uint32_t inst) {
    csrWrite(csr(inst), CSRegisters[csr(inst)] | rs1(inst));
    debug("csrrsi\n");
}
void CPU::exec_CSRRCI(uint32_t inst) {
    csrWrite(csr(inst), CSRegisters[csr(inst)] & !rs1(inst));
    debug("csrrci\n");
}
void CPU::exec_SFENCE_VMA(uint32_t inst) {
    // Do Nothing
    debug("sfence.vma\n");
}
void CPU::exec_SRET(uint32_t inst) {
    switch ((csrRead(SSTATUS) >> 8) & 1) {
        case 1: CurrentMode = Mode::Supervisor; break;
        default: CurrentMode = Mode::User; break;
    };

    if (((csrRead(SSTATUS) >> 5) & 1) == 1) {
        csrWrite(SSTATUS, (csrRead(SSTATUS) | (1 << 1)));
    } else {
        csrWrite(SSTATUS, (csrRead(SSTATUS) & !(1 << 1)));
    }

    csrWrite(SSTATUS, csrRead(SSTATUS) | (1 << 5));
    csrWrite(SSTATUS, csrRead(SSTATUS) & !(1 << 8));
    debug("sret\n");
}
void CPU::exec_MRET(uint32_t inst) {
    ProgramCounter = csrRead(MEPC);
    
    switch ((csrRead(MSTATUS) >> 11) & 0b11) {
        case 2: CurrentMode = Mode::Machine; break;
        case 1: CurrentMode = Mode::Supervisor; break;
        default: CurrentMode = Mode::User; break;
    };

    if (((csrRead(MSTATUS) >> 7) & 1) == 1) {
        csrWrite(MSTATUS, (csrRead(MSTATUS) | (1 << 3)));
    } else {
        csrWrite(MSTATUS, (csrRead(MSTATUS) & !(1 << 3)));
    }

    csrWrite(MSTATUS, (csrRead(MSTATUS) | (1 << 7)));

    csrWrite(MSTATUS, (csrRead(MSTATUS) & !(0b11 << 11)));
    debug("mret\n");
}
void CPU::exec_MUL(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] * Registers[rs2(inst)];
    debug("mul\n");
}
void CPU::exec_MULH(uint32_t inst) {
    // TODO
    debug("mulh\n");
}
void CPU::exec_MULHSU(uint32_t inst) {
    // TODO
    debug("mulhsu\n");
}
void CPU::exec_MULHU(uint32_t inst) {
    // TODO
    debug("mulhu\n");
}
void CPU::exec_DIV(uint32_t inst) {
    // TODO
    debug("div\n");
}
void CPU::exec_DIVU(uint32_t inst) {
    switch (Registers[rs2(inst)]) {
    case 0: {
        // TODO: Set DZ (Divide by Zero) in the FCSR csr flag to 1.
        Registers[rd(inst)] = 0xffffffffffffffff;
    } break;
    default: {
        uint64_t dividend = Registers[rs1(inst)];
        uint64_t divisor = Registers[rs2(inst)];
        Registers[rd(inst)] = dividend / divisor;
    } break;
    };
    debug("divu\n");
}
void CPU::exec_REM(uint32_t inst) {
    // TODO
    debug("rem\n");
}
void CPU::exec_REMU(uint32_t inst) {
    // TODO
    debug("remu\n");
}