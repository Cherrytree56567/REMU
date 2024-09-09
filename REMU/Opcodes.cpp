#include "CPU.h"

void CPU::exec_LUI(uint64_t inst) {
    // LUI places upper 20 bits of U-immediate value to rd
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(inst & 0xfffff000)));
    debug(Info, "lui\n");
}

void CPU::exec_AUIPC(uint64_t inst) {
    // AUIPC forms a 32-bit offset from the 20 upper bits 
    // of the U-immediate
    uint64_t imm = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(inst & 0xfffff000)));
    Registers[rd(inst)] = (ProgramCounter + imm) - 4;
    debug(Info, "auipc\n");
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
    /*debug(Info, "JAL-> rd:%ld, pc:%lx\n", rd(inst), ProgramCounter);*/
    ProgramCounter = (ProgramCounter + imm) - 4;
    debug(Info, "jal\n");
}

void CPU::exec_JALR(uint64_t inst) {
    uint64_t tmp = ProgramCounter;
    uint64_t imm = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(inst & 0xfff00000)) >> 20);
    ProgramCounter = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = tmp;
    debug(Info, "jalr\n");
}

void CPU::exec_BEQ(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] == Registers[rs2(inst)]) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug(Info, "beq\n");
}
void CPU::exec_BNE(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] != Registers[rs2(inst)]) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug(Info, "bne\n");
}
void CPU::exec_BLT(uint64_t inst) {
    /*debug(Info, "Operation: BLT\n");*/
    uint64_t imm = imm_B(inst);
    if (static_cast<int64_t>(Registers[rs1(inst)]) < static_cast<int64_t>(Registers[rs2(inst)])) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug(Info, "blt\n");
}
void CPU::exec_BGE(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (static_cast<int64_t>(Registers[rs1(inst)]) >= static_cast<int64_t>(Registers[rs2(inst)])) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug(Info, "bge\n");
}
void CPU::exec_BLTU(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] < Registers[rs2(inst)]) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug(Info, "bltu\n");
}
void CPU::exec_BGEU(uint64_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] >= Registers[rs2(inst)]) {
        ProgramCounter = (ProgramCounter + imm) - 4;
    }
    debug(Info, "bgeu\n");
}
void CPU::exec_LB(uint64_t inst) {
    // load 1 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(HandleException(MemoryLoad(addr, 8)))));
    debug(Info, "lb\n");
}
void CPU::exec_LH(uint64_t inst) {
    // load 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(HandleException(MemoryLoad(addr, 16)))));
    debug(Info, "lh\n");
}
void CPU::exec_LW(uint64_t inst) {
    // load 4 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(HandleException(MemoryLoad(addr, 32)))));
    debug(Info, "lw\n");
}
void CPU::exec_LD(uint64_t inst) {
    // load 8 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = HandleException(MemoryLoad(addr, 64));
    debug(Info, "ld\n");
}
void CPU::exec_LBU(uint64_t inst) {
    // load unsigned 1 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = HandleException(MemoryLoad(addr, 8));
    debug(Info, "lbu\n");
}
void CPU::exec_LHU(uint64_t inst) {
    // load unsigned 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = HandleException(MemoryLoad(addr, 16));
    debug(Info, "lhu\n");
}
void CPU::exec_LWU(uint64_t inst) {
    // load unsigned 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    Registers[rd(inst)] = HandleException(MemoryLoad(addr, 32));
    debug(Info, "lwu\n");
}
void CPU::exec_SB(uint64_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    MemoryStore(addr, 8, Registers[rs2(inst)]);
    debug(Info, "sb\n");
}
void CPU::exec_SH(uint64_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    MemoryStore(addr, 16, Registers[rs2(inst)]);
    debug(Info, "sh\n");
}
void CPU::exec_SW(uint64_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    MemoryStore(addr, 32, Registers[rs2(inst)]);
    debug(Info, "sw\n");
}
void CPU::exec_SD(uint64_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + imm;
    MemoryStore(addr, 64, Registers[rs2(inst)]);
    debug(Info, "sd\n");
}

void CPU::exec_ADDI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] + imm;
    debug(Info, "addi\n");
}

void CPU::exec_SLLI(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] << shamt(inst);
    debug(Info, "slli\n");
}

void CPU::exec_SLTI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (static_cast<int64_t>(Registers[rs1(inst)]) < static_cast<int64_t>(imm)) ? 1 : 0;
    debug(Info, "slti\n");
}

void CPU::exec_SLTIU(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (Registers[rs1(inst)] < imm) ? 1 : 0;
    debug(Info, "sltiu\n");
}

void CPU::exec_XORI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] ^ imm;
    debug(Info, "xori\n");
}

void CPU::exec_SRLI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = wrapping_shr(Registers[rs1(inst)], imm);
    debug(Info, "srli\n");
}

void CPU::exec_SRAI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = static_cast<uint64_t>(wrapping_shr(static_cast<int64_t>(Registers[rs1(inst)]), imm));
    debug(Info, "srai\n");
}

void CPU::exec_ORI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] | imm;
    debug(Info, "ori\n");
}

void CPU::exec_ANDI(uint64_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] & imm;
    debug(Info, "andi\n");
}

void CPU::exec_ADD(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] + Registers[rs2(inst)];
    debug(Info, "add\n");
}

void CPU::exec_SUB(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] - Registers[rs2(inst)];
    debug(Info, "sub\n");
}

void CPU::exec_SLL(uint64_t inst) {
    Registers[rd(inst)] = wrapping_shl(Registers[rs1(inst)], static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f)));
    debug(Info, "sll\n");
}

void CPU::exec_SLT(uint64_t inst) {
    Registers[rd(inst)] = (static_cast<int64_t>(Registers[rs1(inst)]) < static_cast<int32_t>(Registers[rs2(inst)])) ? 1 : 0;
    debug(Info, "slt\n");
}

void CPU::exec_SLTU(uint64_t inst) {
    Registers[rd(inst)] = (Registers[rs1(inst)] < Registers[rs2(inst)]) ? 1 : 0;
    debug(Info, "slti\n");
}

void CPU::exec_XOR(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] ^ Registers[rs2(inst)];
    debug(Info, "xor\n");
}

void CPU::exec_SRL(uint64_t inst) {
    Registers[rd(inst)] = wrapping_shr(Registers[rs1(inst)], static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f)));
    debug(Info, "srl\n");
}

void CPU::exec_SRA(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(wrapping_shr(static_cast<int64_t>(Registers[rs1(inst)]), static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f))));
    debug(Info, "sra\n");
}

void CPU::exec_OR(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] | Registers[rs2(inst)];
    debug(Info, "or\n");
}

void CPU::exec_AND(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] & Registers[rs2(inst)];
    debug(Info, "and\n");
}

void CPU::exec_FENCE(uint64_t inst) {
    debug(Info, "fence\n");
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
    debug(Info, "addiw\n");
}

void CPU::exec_SLLIW(uint64_t inst) {
    Registers[rd(inst)] = wrapping_shl(Registers[rs1(inst)], static_cast<uint32_t>(imm_I(inst) & 0x1f));
    debug(Info, "slliw\n");
}
void CPU::exec_SRLIW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<int32_t>(wrapping_shr(static_cast<uint64_t>(Registers[rs1(inst)]), static_cast<uint32_t>(imm_I(inst) & 0x1f)));
    debug(Info, "srliw\n");
}
void CPU::exec_SRAIW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(wrapping_shr(static_cast<int32_t>(Registers[rs1(inst)]), static_cast<uint32_t>(imm_I(inst) & 0x1f))));
    debug(Info, "sraiw\n");
}
void CPU::exec_ADDW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(Registers[rs1(inst)] + Registers[rs2(inst)])));
    debug(Info, "addw\n");
}
void CPU::exec_MULW(uint64_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        * (int64_t)Registers[rs2(inst)]);
    debug(Info, "mulw\n");
}
void CPU::exec_SUBW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(Registers[rs1(inst)] - Registers[rs2(inst)]));
    debug(Info, "subw\n");
}
void CPU::exec_DIVW(uint64_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        / (int64_t)Registers[rs2(inst)]);
    debug(Info, "divw\n");
}
void CPU::exec_SLLW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(wrapping_shl(static_cast<uint32_t>(Registers[rs1(inst)]), static_cast<uint32_t>(Registers[rs2(inst)] & 0x1f))));
    debug(Info, "sllw\n");
}
void CPU::exec_SRLW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(wrapping_shr(static_cast<uint32_t>(Registers[rs1(inst)]), static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f)))));
    debug(Info, "srlw\n");
}
void CPU::exec_DIVUW(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] / (int64_t)Registers[rs2(inst)];
    debug(Info, "divuw\n");
}
void CPU::exec_SRAW(uint64_t inst) {
    Registers[rd(inst)] = static_cast<uint64_t>(static_cast<int32_t>(Registers[rs1(inst)]) >> static_cast<int32_t>(static_cast<uint32_t>(static_cast<uint64_t>(Registers[rs2(inst)] & 0x3f))));
    debug(Info, "sraw\n");
}
void CPU::exec_REMW(uint64_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        % (int64_t)Registers[rs2(inst)]);
    debug(Info, "remw\n");
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
    debug(Info, "remuw\n");
}

void CPU::exec_LR_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    debug(Info, "amolr.w\n");
}
void CPU::exec_SC_W(uint64_t inst) {
    MemoryStore(Registers[rs1(inst)], 32, Registers[rs2(inst)]);
    Registers[rd(inst)] = 0;
    debug(Info, "amosc.w\n");
}
void CPU::exec_AMOSWAP_W(uint64_t inst) {
    MemoryStore(rs1(inst), 32, rs2(inst));
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    debug(Info, "amoswap.w\n");
}
void CPU::exec_AMOADD_W(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    uint64_t res = tmp + Registers[rs2(inst)];
    MemoryStore(Registers[rs1(inst)], 32, res);
    Registers[rd(inst)] = tmp;
    debug(Info, "amoadd.w\n");
}
void CPU::exec_AMOXOR_W(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    uint64_t res = tmp ^ (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug(Info, "amoxor.w\n");
}
void CPU::exec_AMOAND_W(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    uint64_t res = tmp & (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug(Info, "amoand.w\n");
}
void CPU::exec_AMOOR_W(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 32));
    uint64_t res = tmp | (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug(Info, "amoor.w\n");
}
void CPU::exec_AMOMIN_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::min(Registers[rd(inst)], rs2(inst)));
    debug(Info, "amomin.w\n");
}
void CPU::exec_AMOMAX_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::max(Registers[rd(inst)], rs2(inst)));
    debug(Info, "amomax.w\n");
}
void CPU::exec_AMOMINU_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::min(Registers[rd(inst)], rs2(inst)));
    debug(Info, "amominu.w\n");
}
void CPU::exec_AMOMAXU_W(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 32));
    MemoryStore(rs1(inst), 32, std::max(Registers[rd(inst)], rs2(inst)));
    debug(Info, "amomaxu.w\n");
}

void CPU::exec_LR_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    debug(Info, "amolr.d\n");
}
void CPU::exec_SC_D(uint64_t inst) {
    MemoryStore(Registers[rs1(inst)], 64, Registers[rs2(inst)]);
    Registers[rd(inst)] = 0;
    debug(Info, "amosc.d\n");
}
void CPU::exec_AMOSWAP_D(uint64_t inst) {
    MemoryStore(rs1(inst), 64, rs2(inst));
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    debug(Info, "amoswap.d\n");
}
void CPU::exec_AMOADD_D(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp + Registers[rs2(inst)];
    MemoryStore(Registers[rs1(inst)], 64, res);
    Registers[rd(inst)] = tmp;
    debug(Info, "amoadd.d\n");
}
void CPU::exec_AMOXOR_D(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp ^ (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug(Info, "amoxor.d\n");
}
void CPU::exec_AMOAND_D(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp & (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug(Info, "amoand.d\n");
}
void CPU::exec_AMOOR_D(uint64_t inst) {
    uint64_t tmp = HandleException(MemoryLoad(Registers[rs1(inst)], 64));
    uint64_t res = tmp | (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug(Info, "amoor.d\n");
}
void CPU::exec_AMOMIN_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::min(Registers[rd(inst)], rs2(inst)));
    debug(Info, "amomin.d\n");
}
void CPU::exec_AMOMAX_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::max(Registers[rd(inst)], rs2(inst)));
    debug(Info, "amomax.d\n");
}
void CPU::exec_AMOMINU_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::min(Registers[rd(inst)], rs2(inst)));
    debug(Info, "amominu.d\n");
}
void CPU::exec_AMOMAXU_D(uint64_t inst) {
    Registers[rd(inst)] = HandleException(MemoryLoad(rs1(inst), 64));
    MemoryStore(rs1(inst), 64, std::max(Registers[rd(inst)], rs2(inst)));
    debug(Info, "amomaxu.d\n");
}

void CPU::exec_CSRRW(uint64_t inst) {
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), Registers[rs1(inst)]);
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug(Info, "csrrw\n");
}
void CPU::exec_CSRRS(uint64_t inst) {
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), t | Registers[rs1(inst)]);
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug(Info, "csrrs\n");
}
void CPU::exec_CSRRC(uint64_t inst) {
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), t & (!Registers[rs1(inst)]));
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug(Info, "csrrc\n");
}
void CPU::exec_CSRRWI(uint64_t inst) {
    uint64_t zimm = rs1(inst);
    Registers[rd(inst)] = csrRead(csr(inst));
    csrWrite(csr(inst), zimm);

    update_paging(csr(inst));
    debug(Info, "csrrwi\n");
}
void CPU::exec_CSRRSI(uint64_t inst) {
    uint64_t zimm = rs1(inst);
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), t | zimm);
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug(Info, "csrrsi\n");
}
void CPU::exec_CSRRCI(uint64_t inst) {
    uint64_t zimm = rs1(inst);
    uint64_t t = csrRead(csr(inst));
    csrWrite(csr(inst), t & (!zimm));
    Registers[rd(inst)] = t;

    update_paging(csr(inst));
    debug(Info, "csrrci\n");
}
void CPU::exec_SFENCE_VMA(uint64_t inst) {
    // Do Nothing
    debug(Info, "sfence.vma\n");
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
    debug(Info, "sret\n");
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
    debug(Info, "mret\n");
}
void CPU::exec_MUL(uint64_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] * Registers[rs2(inst)];
    debug(Info, "mul\n");
}
void CPU::exec_MULH(uint64_t inst) {
    // TODO
    debug(Info, "mulh\n");
}
void CPU::exec_MULHSU(uint64_t inst) {
    // TODO
    debug(Info, "mulhsu\n");
}
void CPU::exec_MULHU(uint64_t inst) {
    // TODO
    debug(Info, "mulhu\n");
}
void CPU::exec_DIV(uint64_t inst) {
    // TODO
    debug(Info, "div\n");
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
    debug(Info, "divu\n");
}
void CPU::exec_REM(uint64_t inst) {
    // TODO
    debug(Info, "rem\n");
}
void CPU::exec_REMU(uint64_t inst) {
    // TODO
    debug(Info, "remu\n");
}