#define CPU_NAME CPU

#include "CPU.h"

void CPU_NAME::exec_LUI(uint32_t inst) {
    // LUI places upper 20 bits of U-immediate value to rd
    Registers[rd(inst)] = (uint64_t)(int64_t)(int32_t)(inst & 0xfffff000);
    debug("lui\n");
}

void CPU_NAME::exec_AUIPC(uint32_t inst) {
    // AUIPC forms a 32-bit offset from the 20 upper bits 
    // of the U-immediate
    uint64_t imm = imm_U(inst);
    Registers[rd(inst)] = ((int64_t)ProgramCounter + (int64_t)imm) - 4;
    debug("auipc\n");
}

void CPU_NAME::exec_JAL(uint32_t inst) {
    uint64_t imm = imm_J(inst);
    Registers[rd(inst)] = ProgramCounter;
    /*debug("JAL-> rd:%ld, pc:%lx\n", rd(inst), ProgramCounter);*/
    ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("jal\n");
}

void CPU_NAME::exec_JALR(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    uint64_t tmp = ProgramCounter;
    ProgramCounter = (Registers[rs1(inst)] + (int64_t)imm) & 0xfffffffe;
    Registers[rd(inst)] = tmp;
    /*debug("NEXT -> %#lx, imm:%#lx\n", ProgramCounter, imm);*/
    debug("jalr\n");
}

void CPU_NAME::exec_BEQ(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if ((int64_t)Registers[rs1(inst)] == (int64_t)Registers[rs2(inst)])
        ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("beq\n");
}
void CPU_NAME::exec_BNE(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if ((int64_t)Registers[rs1(inst)] != (int64_t)Registers[rs2(inst)])
        ProgramCounter = (ProgramCounter + (int64_t)imm - 4);
    debug("bne\n");
}
void CPU_NAME::exec_BLT(uint32_t inst) {
    /*debug("Operation: BLT\n");*/
    uint64_t imm = imm_B(inst);
    if ((int64_t)Registers[rs1(inst)] < (int64_t)Registers[rs2(inst)])
        ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("blt\n");
}
void CPU_NAME::exec_BGE(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if ((int64_t)Registers[rs1(inst)] >= (int64_t)Registers[rs2(inst)])
        ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("bge\n");
}
void CPU_NAME::exec_BLTU(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] < Registers[rs2(inst)])
        ProgramCounter = ProgramCounter + (int64_t)imm - 4;
    debug("bltu\n");
}
void CPU_NAME::exec_BGEU(uint32_t inst) {
    uint64_t imm = imm_B(inst);
    if (Registers[rs1(inst)] >= Registers[rs2(inst)])
        ProgramCounter = (int64_t)ProgramCounter + (int64_t)imm - 4;
    debug("jal\n");
}
void CPU_NAME::exec_LB(uint32_t inst) {
    // load 1 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = (int64_t)(int8_t)MemoryLoad(addr, 8);
    debug("lb\n");
}
void CPU_NAME::exec_LH(uint32_t inst) {
    // load 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = (int64_t)(int16_t)MemoryLoad(addr, 16);
    debug("lh\n");
}
void CPU_NAME::exec_LW(uint32_t inst) {
    // load 4 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = (int64_t)(int32_t)MemoryLoad(addr, 32);
    debug("lw\n");
}
void CPU_NAME::exec_LD(uint32_t inst) {
    // load 8 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = (int64_t)MemoryLoad(addr, 64);
    debug("ld\n");
}
void CPU_NAME::exec_LBU(uint32_t inst) {
    // load unsigned 1 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = MemoryLoad(addr, 8);
    debug("lbu\n");
}
void CPU_NAME::exec_LHU(uint32_t inst) {
    // load unsigned 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = MemoryLoad(addr, 16);
    debug("lhu\n");
}
void CPU_NAME::exec_LWU(uint32_t inst) {
    // load unsigned 2 byte to rd from address in rs1
    uint64_t imm = imm_I(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    Registers[rd(inst)] = MemoryLoad(addr, 32);
    debug("lwu\n");
}
void CPU_NAME::exec_SB(uint32_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    MemoryStore(addr, 8, Registers[rs2(inst)]);
    debug("sb\n");
}
void CPU_NAME::exec_SH(uint32_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    MemoryStore(addr, 16, Registers[rs2(inst)]);
    debug("sh\n");
}
void CPU_NAME::exec_SW(uint32_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    MemoryStore(addr, 32, Registers[rs2(inst)]);
    debug("sw\n");
}
void CPU_NAME::exec_SD(uint32_t inst) {
    uint64_t imm = imm_S(inst);
    uint64_t addr = Registers[rs1(inst)] + (int64_t)imm;
    MemoryStore(addr, 64, Registers[rs2(inst)]);
    debug("sd\n");
}

void CPU_NAME::exec_ADDI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] + (int64_t)imm;
    debug("addi\n");
}

void CPU_NAME::exec_SLLI(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] << shamt(inst);
    debug("slli\n");
}

void CPU_NAME::exec_SLTI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (Registers[rs1(inst)] < (int64_t)imm) ? 1 : 0;
    debug("slti\n");
}

void CPU_NAME::exec_SLTIU(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (Registers[rs1(inst)] < imm) ? 1 : 0;
    debug("sltiu\n");
}

void CPU_NAME::exec_XORI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] ^ imm;
    debug("xori\n");
}

void CPU_NAME::exec_SRLI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] >> imm;
    debug("srli\n");
}

void CPU_NAME::exec_SRAI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (int32_t)Registers[rs1(inst)] >> imm;
    debug("srai\n");
}

void CPU_NAME::exec_ORI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] | imm;
    debug("ori\n");
}

void CPU_NAME::exec_ANDI(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] & imm;
    debug("andi\n");
}

void CPU_NAME::exec_ADD(uint32_t inst) {
    Registers[rd(inst)] =
        (uint64_t)((int64_t)Registers[rs1(inst)] + (int64_t)Registers[rs2(inst)]);
    debug("add\n");
}

void CPU_NAME::exec_SUB(uint32_t inst) {
    Registers[rd(inst)] =
        (uint64_t)((int64_t)Registers[rs1(inst)] - (int64_t)Registers[rs2(inst)]);
    debug("sub\n");
}

void CPU_NAME::exec_SLL(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] << (int64_t)Registers[rs2(inst)];
    debug("sll\n");
}

void CPU_NAME::exec_SLT(uint32_t inst) {
    Registers[rd(inst)] = (Registers[rs1(inst)] < (int64_t)Registers[rs2(inst)]) ? 1 : 0;
    debug("slt\n");
}

void CPU_NAME::exec_SLTU(uint32_t inst) {
    Registers[rd(inst)] = (Registers[rs1(inst)] < Registers[rs2(inst)]) ? 1 : 0;
    debug("slti\n");
}

void CPU_NAME::exec_XOR(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] ^ Registers[rs2(inst)];
    debug("xor\n");
}

void CPU_NAME::exec_SRL(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] >> Registers[rs2(inst)];
    debug("srl\n");
}

void CPU_NAME::exec_SRA(uint32_t inst) {
    Registers[rd(inst)] = (int32_t)Registers[rs1(inst)] >>
        (int64_t)Registers[rs2(inst)];
    debug("sra\n");
}

void CPU_NAME::exec_OR(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] | Registers[rs2(inst)];
    debug("or\n");
}

void CPU_NAME::exec_AND(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] & Registers[rs2(inst)];
    debug("and\n");
}

void CPU_NAME::exec_FENCE(uint32_t inst) {
    debug("fence\n");
}

void CPU_NAME::exec_ECALL(uint32_t inst) {}
void CPU_NAME::exec_EBREAK(uint32_t inst) {}

void CPU_NAME::exec_ECALLBREAK(uint32_t inst) {
    if (imm_I(inst) == 0x0)
        exec_ECALL(inst);
    if (imm_I(inst) == 0x1)
        exec_EBREAK(inst);
    debug("ecallbreak\n");
}


void CPU_NAME::exec_ADDIW(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = Registers[rs1(inst)] + (int64_t)imm;
    debug("addiw\n");
}

void CPU_NAME::exec_SLLIW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] << shamt(inst));
    debug("slliw\n");
}
void CPU_NAME::exec_SRLIW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] >> shamt(inst));
    debug("srliw\n");
}
void CPU_NAME::exec_SRAIW(uint32_t inst) {
    uint64_t imm = imm_I(inst);
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] >> (uint64_t)(int64_t)(int32_t)imm);
    debug("sraiw\n");
}
void CPU_NAME::exec_ADDW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        + (int64_t)Registers[rs2(inst)]);
    debug("addw\n");
}
void CPU_NAME::exec_MULW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        * (int64_t)Registers[rs2(inst)]);
    debug("mulw\n");
}
void CPU_NAME::exec_SUBW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        - (int64_t)Registers[rs2(inst)]);
    debug("subw\n");
}
void CPU_NAME::exec_DIVW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        / (int64_t)Registers[rs2(inst)]);
    debug("divw\n");
}
void CPU_NAME::exec_SLLW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] << Registers[rs2(inst)]);
    debug("sllw\n");
}
void CPU_NAME::exec_SRLW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] >> Registers[rs2(inst)]);
    debug("srlw\n");
}
void CPU_NAME::exec_DIVUW(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] / (int64_t)Registers[rs2(inst)];
    debug("divuw\n");
}
void CPU_NAME::exec_SRAW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)] >> (uint64_t)(int64_t)(int32_t)Registers[rs2(inst)]);
    debug("sraw\n");
}
void CPU_NAME::exec_REMW(uint32_t inst) {
    Registers[rd(inst)] = (int64_t)(int32_t)(Registers[rs1(inst)]
        % (int64_t)Registers[rs2(inst)]);
    debug("remw\n");
}
void CPU_NAME::exec_REMUW(uint32_t inst) {
    Registers[rd(inst)] = Registers[rs1(inst)] % (int64_t)Registers[rs2(inst)];
    debug("remuw\n");
}

void CPU_NAME::exec_LR_W(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(Registers[rs1(inst)], 32);
    debug("amolr.w\n");
}
void CPU_NAME::exec_SC_W(uint32_t inst) {
    MemoryStore(Registers[rs1(inst)], 32, Registers[rs2(inst)]);
    Registers[rd(inst)] = 0;
    debug("amosc.w\n");
}
void CPU_NAME::exec_AMOSWAP_W(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 32);
    MemoryStore(rs1(inst), 32, rs2(inst));
    debug("amoswap.w\n");
}
void CPU_NAME::exec_AMOADD_W(uint32_t inst) {
    uint32_t tmp = MemoryLoad(Registers[rs1(inst)], 32);
    uint32_t res = tmp + (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoadd.w\n");
}
void CPU_NAME::exec_AMOXOR_W(uint32_t inst) {
    uint32_t tmp = MemoryLoad(Registers[rs1(inst)], 32);
    uint32_t res = tmp ^ (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoxor.w\n");
}
void CPU_NAME::exec_AMOAND_W(uint32_t inst) {
    uint32_t tmp = MemoryLoad(Registers[rs1(inst)], 32);
    uint32_t res = tmp & (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoand.w\n");
}
void CPU_NAME::exec_AMOOR_W(uint32_t inst) {
    uint32_t tmp = MemoryLoad(Registers[rs1(inst)], 32);
    uint32_t res = tmp | (uint32_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 32, res);
    debug("amoor.w\n");
}
void CPU_NAME::exec_AMOMIN_W(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 32);
    MemoryStore(rs1(inst), 32, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amomin.w\n");
}
void CPU_NAME::exec_AMOMAX_W(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 32);
    MemoryStore(rs1(inst), 32, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomax.w\n");
}
void CPU_NAME::exec_AMOMINU_W(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 32);
    MemoryStore(rs1(inst), 32, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amominu.w\n");
}
void CPU_NAME::exec_AMOMAXU_W(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 32);
    MemoryStore(rs1(inst), 32, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomaxu.w\n");
}

void CPU_NAME::exec_LR_D(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(Registers[rs1(inst)], 64);
    debug("amolr.d\n");
}
void CPU_NAME::exec_SC_D(uint32_t inst) {
    MemoryStore(Registers[rs1(inst)], 64, Registers[rs2(inst)]);
    Registers[rd(inst)] = 0;
    debug("amosc.d\n");
}
void CPU_NAME::exec_AMOSWAP_D(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 64);
    MemoryStore(rs1(inst), 64, rs2(inst));
    debug("amoswap.d\n");
}
void CPU_NAME::exec_AMOADD_D(uint32_t inst) {
    uint64_t tmp = MemoryLoad(Registers[rs1(inst)], 64);
    uint64_t res = tmp + (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoadd.d\n");
}
void CPU_NAME::exec_AMOXOR_D(uint32_t inst) {
    uint64_t tmp = MemoryLoad(Registers[rs1(inst)], 64);
    uint64_t res = tmp ^ (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoxor.d\n");
}
void CPU_NAME::exec_AMOAND_D(uint32_t inst) {
    uint64_t tmp = MemoryLoad(Registers[rs1(inst)], 64);
    uint64_t res = tmp & (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoand.d\n");
}
void CPU_NAME::exec_AMOOR_D(uint32_t inst) {
    uint64_t tmp = MemoryLoad(Registers[rs1(inst)], 64);
    uint64_t res = tmp | (uint64_t)Registers[rs2(inst)];
    Registers[rd(inst)] = tmp;
    MemoryStore(Registers[rs1(inst)], 64, res);
    debug("amoor.d\n");
}
void CPU_NAME::exec_AMOMIN_D(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 64);
    MemoryStore(rs1(inst), 64, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amomin.d\n");
}
void CPU_NAME::exec_AMOMAX_D(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 64);
    MemoryStore(rs1(inst), 64, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomax.d\n");
}
void CPU_NAME::exec_AMOMINU_D(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 64);
    MemoryStore(rs1(inst), 64, std::min(Registers[rd(inst)], rs2(inst)));
    debug("amominu.d\n");
}
void CPU_NAME::exec_AMOMAXU_D(uint32_t inst) {
    Registers[rd(inst)] = MemoryLoad(rs1(inst), 64);
    MemoryStore(rs1(inst), 64, std::max(Registers[rd(inst)], rs2(inst)));
    debug("amomaxu.d\n");
}

void CPU_NAME::exec_CSRRW(uint32_t inst) {
    Registers[rd(inst)] = csrRead(csr(inst));
    csrWrite(csr(inst), Registers[rs1(inst)]);
    debug("csrrw\n");
}
void CPU_NAME::exec_CSRRS(uint32_t inst) {
    csrWrite(csr(inst), CSRegisters[csr(inst)] | Registers[rs1(inst)]);
    debug("csrrs\n");
}
void CPU_NAME::exec_CSRRC(uint32_t inst) {
    csrWrite(csr(inst), CSRegisters[csr(inst)] & !(Registers[rs1(inst)]));
    debug("csrrc\n");
}
void CPU_NAME::exec_CSRRWI(uint32_t inst) {
    csrWrite(csr(inst), rs1(inst));
    debug("csrrwi\n");
}
void CPU_NAME::exec_CSRRSI(uint32_t inst) {
    csrWrite(csr(inst), CSRegisters[csr(inst)] | rs1(inst));
    debug("csrrsi\n");
}
void CPU_NAME::exec_CSRRCI(uint32_t inst) {
    csrWrite(csr(inst), CSRegisters[csr(inst)] & !rs1(inst));
    debug("csrrci\n");
}