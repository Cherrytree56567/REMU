#include "CPU.h"

void CPU::exec_LUI(uint64_t inst) {
    // LUI places upper 20 bits of U-immediate value to rd
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(inst & 0xfffff000)));
    debug("lui\n");
}

void CPU::exec_AUIPC(uint64_t inst) {
    // AUIPC forms a 32-bit offset from the 20 upper bits 
    // of the U-immediate
    uint64_t imm = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(inst & 0xfffff000)));
    Registers[rd(inst)] = (ProgramCounter + imm) - 4;
    debug("auipc\n");
}

uint32_t wrapping_add(uint32_t a, uint64_t b) {
    // Calculate the sum
    uint64_t sum = static_cast<uint64_t>(a) + b;

    // Wrap around if the sum exceeds UINT32_MAX
    return static_cast<uint32_t>(sum & 0xfffff10e);
}

void CPU::exec_JAL(uint64_t inst) {
    Registers[rd(inst)] = ProgramCounter;
    uint64_t imm = imm_J(inst);
    /*debug("JAL-> rd:%ld, pc:%lx\n", rd(inst), ProgramCounter);*/
    ProgramCounter = (ProgramCounter + imm) - 4;
    debug("jal\n");
}

void CPU::exec_JALR(uint64_t inst) {
    uint64_t tmp = ProgramCounter;
    uint64_t imm = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(inst & 0xfff00000)) >> 20);
    ProgramCounter = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = tmp;
    debug("jalr\n");
}

void CPU::exec_BEQ(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] == Registers[rs2(inst)]) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug("beq\n");
}
void CPU::exec_BNE(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] != Registers[rs2(inst)]) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug("bne\n");
}
void CPU::exec_BLT(uint64_t inst) {
    /*debug("Operation: BLT\n");*/
    uint64_t imm = imm_B(inst);
    if (static_cast<int64_t>(Registers[rs1(inst)]) < static_cast<int64_t>(Registers[rs2(inst)])) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug("blt\n");
}
void CPU::exec_BGE(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (static_cast<int64_t>(Registers[rs1(inst)]) >= static_cast<int64_t>(Registers[rs2(inst)])) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug("bge\n");
}
void CPU::exec_BLTU(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] < Registers[rs2(inst)]) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug("bltu\n");
}
void CPU::exec_BGEU(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] >= Registers[rs2(inst)]) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug("bgeu\n");
}
void CPU::exec_LB(uint64_t inst) {
    // load 1 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(HandleException(MemoryLoad(addr, 8)))));
    debug("lb\n");
}
void CPU::exec_LH(uint64_t inst) {
    // load 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(HandleException(MemoryLoad(addr, 16)))));
    debug("lh\n");
}
void CPU::exec_LW(uint64_t inst) {
    // load 4 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(HandleException(MemoryLoad(addr, 32)))));
    debug("lw\n");
}
void CPU::exec_LD(uint64_t inst) {
    // load 8 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = HandleException(MemoryLoad(addr, 64));
    debug("ld\n");
}
void CPU::exec_LBU(uint64_t inst) {
    // load unsigned 1 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = HandleException(MemoryLoad(addr, 8));
    debug("lbu\n");
}
void CPU::exec_LHU(uint64_t inst) {
    // load unsigned 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = HandleException(MemoryLoad(addr, 16));
    debug("lhu\n");
}
void CPU::exec_LWU(uint64_t inst) {
    // load unsigned 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = HandleException(MemoryLoad(addr, 32));
    debug("lwu\n");
}
void CPU::exec_SB(uint64_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    MemoryStore(addr, 8, Registers[rs2(inst)]);
    debug("sb\n");
}
void CPU::exec_SH(uint64_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    MemoryStore(addr, 16, Registers[rs2(inst)]);
    debug("sh\n");
}
void CPU::exec_SW(uint64_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    MemoryStore(addr, 32, Registers[rs2(inst)]);
    debug("sw\n");
}
void CPU::exec_SD(uint64_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    MemoryStore(addr, 64, Registers[rs2(inst)]);
    debug("sd\n");
}

void CPU::exec_ADDI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] + imm;
    debug("addi\n");
}

void CPU::exec_SLLI(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] << shamt(inst);
    debug("slli\n");
}

void CPU::exec_SLTI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (static_cast<int64_t>(Registers[rs1(inst)]) < static_cast<int64_t>(imm)) ? 1 : 0;
    debug("slti\n");
}

void CPU::exec_SLTIU(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (Registers[rs1(inst)] < imm) ? 1 : 0;
    debug("sltiu\n");
}

void CPU::exec_XORI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] ^ imm;
    debug("xori\n");
}

void CPU::exec_SRLI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = wrapping_shr(Registers[rs1(inst)], imm);
    debug("srli\n");
}

void CPU::exec_SRAI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = static_cast<uint64_t>(wrapping_shr(static_cast<int64_t>(Registers[rs1(inst)]), imm));
    debug("srai\n");
}

void CPU::exec_ORI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] | imm;
    debug("ori\n");
}

void CPU::exec_ANDI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] & imm;
    debug("andi\n");
}

void CPU::exec_ADD(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] + Registers[rs2(inst)];
    debug("add\n");
}

void CPU::exec_SUB(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] - Registers[rs2(inst)];
    debug("sub\n");
}

void CPU::exec_SLL(uint64_t inst) {
    Registers[rd(inst)] = wrapping_shl(Registers[rs1(inst)], static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f)));
    debug("sll\n");
}

void CPU::exec_SLT(uint64_t inst) {
    Registers[rd(inst)] = (static_cast<int64_t>(Registers[rs1(inst)]) < static_cast<int32_t>(Registers[rs2(inst)])) ? 1 : 0;
    debug("slt\n");
}

void CPU::exec_SLTU(uint64_t inst) {
    Registers[rd(inst)] = (Registers[rs1(inst)] < Registers[rs2(inst)]) ? 1 : 0;
    debug("slti\n");
}

void CPU::exec_XOR(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] ^ Registers[rs2(inst)];
    debug("xor\n");
}

void CPU::exec_SRL(uint64_t inst) {
    Registers[rd(inst)] = wrapping_shr(Registers[rs1(inst)], static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f)));
    debug("srl\n");
}

void CPU::exec_SRA(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(wrapping_shr(static_cast<int64_t>(Registers[rs1(inst)]), static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f))));
    debug("sra\n");
}

void CPU::exec_OR(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] | Registers[rs2(inst)];
    debug("or\n");
}

void CPU::exec_AND(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] & Registers[rs2(inst)];
    debug("and\n");
}

void CPU::exec_FENCE(uint64_t inst) {
    debug("fence\n");
}

std::variant<uint64_t, Exception> CPU::exec_ECALL(uint64_t inst) {
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
std::variant<uint64_t, Exception> CPU::exec_EBREAK(uint64_t inst) {
    return Exception::Breakpoint;
}

void CPU::exec_ADDIW(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(Registers[rs1(inst)] + imm)));
    debug("addiw\n");
}

void CPU::exec_SLLIW(uint64_t inst) {
    Registers[rd(inst)] = wrapping_shl(Registers[rs1(inst)], static_cast<uint32_t>(imm_I(inst) & 0x1f));
    debug("slliw\n");
}
void CPU::exec_SRLIW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<int32_t>(wrapping_shr(static_cast<uint64_t>(Registers[rs1(inst)]), static_cast<uint32_t>(imm_I(inst) & 0x1f)));
    debug("srliw\n");
}
void CPU::exec_SRAIW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(wrapping_shr(static_cast<int32_t>(Registers[rs1(inst)]), static_cast<uint32_t>(imm_I(inst) & 0x1f))));
    debug("sraiw\n");
}
void CPU::exec_ADDW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(Registers[rs1(inst)] + Registers[rs2(inst)])));
    debug("addw\n");
}
void CPU::exec_MULW(uint64_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        * (int64_t)Registers[rs2(inst)]);
    debug("mulw\n");
}
void CPU::exec_SUBW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(Registers[rs1(inst)] - Registers[rs2(inst)]));
    debug("subw\n");
}
void CPU::exec_DIVW(uint64_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        / (int64_t)Registers[rs2(inst)]);
    debug("divw\n");
}
void CPU::exec_SLLW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(wrapping_shl(static_cast<uint32_t>(Registers[rs1(inst)]), static_cast<uint32_t>(Registers[rs2(inst)] & 0x1f))));
    debug("sllw\n");
}
void CPU::exec_SRLW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(wrapping_shr(static_cast<uint32_t>(Registers[rs1(inst)]), static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f)))));
    debug("srlw\n");
}
void CPU::exec_DIVUW(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] / (int64_t)Registers[rs2(inst)];
    debug("divuw\n");
}
void CPU::exec_SRAW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(Registers[rs1(inst)]) >> static_cast<int32_t>(static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f))));
    debug("sraw\n");
}
void CPU::exec_REMW(uint64_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        % (int64_t)Registers[rs2(inst)]);
    debug("remw\n");
}
void CPU::exec_REMUW(uint64_t inst) {
    switch (Registers[rs2(inst)]) {
    case 0:
        Registers[rd(inst)] = Registers[rs1(inst)];
        break;

    default:
        uint32_t dividend = static_cast<uint32_t>(Registers[rs1(inst)]);
        uint32_t divisor = static_cast<uint32_t>(Registers[rs2(inst)]);
        Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(dividend % divisor));
    }
    debug("remuw\n");
}

void CPU::exec_LR_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    debug("amolr.w\n");
}
void CPU::exec_SC_W(uint64_t inst) {
    MemoryStore(Registers[rs1(inst)], 32, Registers[rs2(inst)]);
    Registers[rd(inst)] = 0;
    debug("amosc.w\n");
}
void CPU::exec_AMOSWAP_W(uint64_t inst) {
    MemoryStore(rs1(inst), 32, rs2(inst));
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    debug("amoswap.w\n");
}
void CPU::exec_AMOADD_W(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    uint64_t res = tmp + Registers[rs2(inst)];
    MemoryStore(Registers[rs1(inst)], 32, res);
    Registers[rd(inst)] = tmp;
    debug("amoadd.w\n");
}
void CPU::exec_AMOXOR_W(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    uint64_t res = tmp ^ (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoxor.w\n");
}
void CPU::exec_AMOAND_W(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    uint64_t res = tmp & (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoand.w\n");
}
void CPU::exec_AMOOR_W(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    uint64_t res = tmp | (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoor.w\n");
}
void CPU::exec_AMOMIN_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amomin.w\n");
}
void CPU::exec_AMOMAX_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomax.w\n");
}
void CPU::exec_AMOMINU_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amominu.w\n");
}
void CPU::exec_AMOMAXU_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomaxu.w\n");
}

void CPU::exec_LR_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    debug("amolr.d\n");
}
void CPU::exec_SC_D(uint64_t inst) {
    MemoryStore(Registers[rs1(inst)], 64, Registers[rs2(inst)]);
    Registers[rd(inst)] = 0;
    debug("amosc.d\n");
}
void CPU::exec_AMOSWAP_D(uint64_t inst) {
    MemoryStore(rs1(inst), 64, rs2(inst));
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    debug("amoswap.d\n");
}
void CPU::exec_AMOADD_D(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp + Registers[rs2(inst)];
    MemoryStore(Registers[rs1(inst)], 64, res);
    Registers[rd(inst)] = tmp;
    debug("amoadd.d\n");
}
void CPU::exec_AMOXOR_D(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp ^ (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoxor.d\n");
}
void CPU::exec_AMOAND_D(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp & (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoand.d\n");
}
void CPU::exec_AMOOR_D(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp | (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoor.d\n");
}
void CPU::exec_AMOMIN_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amomin.d\n");
}
void CPU::exec_AMOMAX_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomax.d\n");
}
void CPU::exec_AMOMINU_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amominu.d\n");
}
void CPU::exec_AMOMAXU_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomaxu.d\n");
}

void CPU::exec_CSRRW(uint64_t inst) {
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), Registers[rs1(inst)]);
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug("csrrw\n");
}
void CPU::exec_CSRRS(uint64_t inst) {
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), t | Registers[rs1(inst)]);
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug("csrrs\n");
}
void CPU::exec_CSRRC(uint64_t inst) {
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), t & (!Registers[rs1(inst)]));
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug("csrrc\n");
}
void CPU::exec_CSRRWI(uint64_t inst) {
    uint64_t zimm = rs1(inst);
    Registers[rd(inst)] = csrRead(csr(inst));
    csrWrite(csr(inst), zimm);

    update_paging(csr(inst));
    debug("csrrwi\n");
}
void CPU::exec_CSRRSI(uint64_t inst) {
    uint64_t zimm = rs1(inst);
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), t | zimm);
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug("csrrsi\n");
}
void CPU::exec_CSRRCI(uint64_t inst) {
    uint64_t zimm = rs1(inst);
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), t & (!zimm));
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug("csrrci\n");
}
void CPU::exec_SFENCE_VMA(uint64_t inst) {
    // Do Nothing
    debug("sfence.vma\n");
}
void CPU::exec_SRET(uint64_t inst) {
    switch ((csrRead(SSTATUS) >> 8) & 1) {
    case 1: CurrentMode = Mode::Supervisor; break;
    default: CurrentMode = Mode::User; break;
    };

    if (((csrRead(SSTATUS) >> 5) & 1) == 1) {
        csrWrite(SSTATUS, (csrRead(SSTATUS) | (1 << 1)));
    }
    else {
        csrWrite(SSTATUS, (csrRead(SSTATUS) & !(1 << 1)));
    }

    csrWrite(SSTATUS, csrRead(SSTATUS) | (1 << 5));
    csrWrite(SSTATUS, csrRead(SSTATUS) & !(1 << 8));
    debug("sret\n");
}
void CPU::exec_MRET(uint64_t inst) {
    ProgramCounter = csrRead(MEPC);

    switch ((csrRead(MSTATUS) >> 11) & 0b11) {
    case 2: CurrentMode = Mode::Machine; break;
    case 1: CurrentMode = Mode::Supervisor; break;
    default: CurrentMode = Mode::User; break;
    };

    if (((csrRead(MSTATUS) >> 7) & 1) == 1) {
        csrWrite(MSTATUS, (csrRead(MSTATUS) | (1 << 3)));
    }
    else {
        csrWrite(MSTATUS, (csrRead(MSTATUS) & !(1 << 3)));
    }

    csrWrite(MSTATUS, (csrRead(MSTATUS) | (1 << 7)));

    csrWrite(MSTATUS, (csrRead(MSTATUS) & !(0b11 << 11)));
    debug("mret\n");
}
void CPU::exec_MUL(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] * Registers[rs2(inst)];
    debug("mul\n");
}
void CPU::exec_MULH(uint64_t inst) {
    // TODO
    debug("mulh\n");
}
void CPU::exec_MULHSU(uint64_t inst) {
    // TODO
    debug("mulhsu\n");
}
void CPU::exec_MULHU(uint64_t inst) {
    // TODO
    debug("mulhu\n");
}
void CPU::exec_DIV(uint64_t inst) {
    // TODO
    debug("div\n");
}
void CPU::exec_DIVU(uint64_t inst) {
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
void CPU::exec_REM(uint64_t inst) {
    // TODO
    debug("rem\n");
}
void CPU::exec_REMU(uint64_t inst) {
    // TODO
    debug("remu\n");
}