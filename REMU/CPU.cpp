#define CPU_NAME CPU
#include "CPU.h"

CPU_NAME::CPU_NAME(std::shared_ptr<RAM> ram) {
    Registers[0] = 0x00; // Register x0 hardwired to 0
    Registers[2] = MEMORY_BASE + MEMORY_SIZE; // Set Stack Pointer
    ProgramCounter = MEMORY_BASE;
    Memory = ram;
}

uint32_t CPU_NAME::Fetch() {
    uint32_t inst = MemoryLoad(ProgramCounter, 32);
    return inst;
}

void CPU_NAME::debug(std::string s) {
    std::cout << s;
}

bool CPU_NAME::Loop() {
    uint32_t inst = Fetch();

    ProgramCounter += 4;

    if (!Execute(inst))
        return false;

    if(ProgramCounter==0)
        return false;

    return true;
}

uint64_t CPU_NAME::MemoryLoad(uint64_t addr, uint64_t size) {
    switch (size) {
    case 8:  return Memory->MemoryLoad8(addr);  break;
    case 16: return Memory->MemoryLoad16(addr); break;
    case 32: return Memory->MemoryLoad32(addr); break;
    case 64: return Memory->MemoryLoad64(addr); break;
    default:;
    }
    return 1;
}

void CPU_NAME::MemoryStore(uint64_t addr, uint64_t size, uint64_t value) {
    switch (size) {
    case 8:  Memory->MemoryStore8(addr, value);  break;
    case 16: Memory->MemoryStore16(addr, value); break;
    case 32: Memory->MemoryStore32(addr, value); break;
    case 64: Memory->MemoryStore64(addr, value); break;
    default:;
    }
}

void CPU_NAME::DumpRegisters() {
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

    for (int i = 0; i < 8; i++) {
        printf("   %4s: %#-13.2lx  ", abi[i], Registers[i]);
        printf("   %2s: %#-13.2lx  ", abi[i + 8], Registers[i + 8]);
        printf("   %2s: %#-13.2lx  ", abi[i + 16], Registers[i + 16]);
        printf("   %3s: %#-13.2lx\n", abi[i + 24], Registers[i + 24]);
    }
}

uint64_t CPU_NAME::csrRead(uint64_t csr) {
    return (uint64_t)(uint32_t)CSRegisters[csr];
}

void CPU_NAME::csrWrite(uint64_t csr, uint64_t value) {
    CSRegisters[csr] = value;
}

int CPU_NAME::Execute(uint32_t inst) {
    int opcode = inst & 0x7f;           // opcode in bits 6..0
    int funct3 = (inst >> 12) & 0x7;    // funct3 in bits 14..12
    int funct7 = (inst >> 25) & 0x7f;   // funct7 in bits 31..25

    Registers[0] = 0;

    switch (opcode) {
        case LUI:   exec_LUI(inst); break;
        case AUIPC: exec_AUIPC(inst); break;

        case JAL:   exec_JAL(inst); break;
        case JALR:  exec_JALR(inst); break;

        case B_TYPE:
            switch (funct3) {
                case BEQ:   exec_BEQ(inst); break;
                case BNE:   exec_BNE(inst); break;
                case BLT:   exec_BLT(inst); break;
                case BGE:   exec_BGE(inst); break;
                case BLTU:  exec_BLTU(inst); break;
                case BGEU:  exec_BGEU(inst); break;
                default: ;
            } break;

        case LOAD:
            switch (funct3) {
                case LB  :  exec_LB(inst); break;  
                case LH  :  exec_LH(inst); break;  
                case LW  :  exec_LW(inst); break;  
                case LD  :  exec_LD(inst); break;  
                case LBU :  exec_LBU(inst); break; 
                case LHU :  exec_LHU(inst); break; 
                case LWU :  exec_LWU(inst); break; 
                default: ;
            } break;

        case S_TYPE:
            switch (funct3) {
                case SB  :  exec_SB(inst); break;  
                case SH  :  exec_SH(inst); break;  
                case SW  :  exec_SW(inst); break;  
                case SD  :  exec_SD(inst); break;  
                default: ;
            } break;

        case I_TYPE:  
            switch (funct3) {
                case ADDI:  exec_ADDI(inst); break;
                case SLLI:  exec_SLLI(inst); break;
                case SLTI:  exec_SLTI(inst); break;
                case SLTIU: exec_SLTIU(inst); break;
                case XORI:  exec_XORI(inst); break;
                case SRI:   
                    switch (funct7) {
                        case SRLI:  exec_SRLI(inst); break;
                        case SRAI:  exec_SRAI(inst); break;
                        default: ;
                    } break;
                case ORI:   exec_ORI(inst); break;
                case ANDI:  exec_ANDI(inst); break;
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

        case R_TYPE:  
            switch (funct3) {
                case ADDSUB:
                    switch (funct7) {
                        case ADD: exec_ADD(inst);
                        case SUB: exec_ADD(inst);
                        default: ;
                    } break;
                case SLL:  exec_SLL(inst); break;
                case SLT:  exec_SLT(inst); break;
                case SLTU: exec_SLTU(inst); break;
                case XOR:  exec_XOR(inst); break;
                case SR:   
                    switch (funct7) {
                        case SRL:  exec_SRL(inst); break;
                        case SRA:  exec_SRA(inst); break;
                        default: ;
                    }
                case OR:   exec_OR(inst); break;
                case AND:  exec_AND(inst); break;
                default:
                    fprintf(stderr, 
                            "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n"
                            , opcode, funct3, funct7);
                    return 0;
            } break;

        case FENCE: exec_FENCE(inst); break;

        case I_TYPE_64:
            switch (funct3) {
                case ADDIW: exec_ADDIW(inst); break;
                case SLLIW: exec_SLLIW(inst); break;
                case SRIW : 
                    switch (funct7) {
                        case SRLIW: exec_SRLIW(inst); break;
                        case SRAIW: exec_SRLIW(inst); break;
                    } break;
            } break;

        case R_TYPE_64:
            switch (funct3) {
                case ADDSUB:
                    switch (funct7) {
                        case ADDW:  exec_ADDW(inst); break;
                        case SUBW:  exec_SUBW(inst); break;
                        case MULW:  exec_MULW(inst); break;
                    } break;
                case DIVW:  exec_DIVW(inst); break;
                case SLLW:  exec_SLLW(inst); break;
                case SRW:
                    switch (funct7) {
                        case SRLW:  exec_SRLW(inst); break;
                        case SRAW:  exec_SRAW(inst); break;
                        case DIVUW: exec_DIVUW(inst); break;
                    } break;
                case REMW:  exec_REMW(inst); break;
                case REMUW: exec_REMUW(inst); break;
                default: ;
            } break;

        case CSR:
            switch (funct3) {
                case ECALLBREAK:    exec_ECALLBREAK(inst); break;
                case CSRRW  :  exec_CSRRW(inst); break;  
                case CSRRS  :  exec_CSRRS(inst); break;  
                case CSRRC  :  exec_CSRRC(inst); break;  
                case CSRRWI :  exec_CSRRWI(inst); break; 
                case CSRRSI :  exec_CSRRSI(inst); break; 
                case CSRRCI :  exec_CSRRCI(inst); break; 
                default:
                    fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
                    return 0;
            } break;

        case AMO_W:
            switch (funct7 >> 2) { // since, funct[1:0] = aq, rl
                case LR_W      :  exec_LR_W(inst); break;  
                case SC_W      :  exec_SC_W(inst); break;  
                case AMOSWAP_W :  exec_AMOSWAP_W(inst); break;  
                case AMOADD_W  :  exec_AMOADD_W(inst); break; 
                case AMOXOR_W  :  exec_AMOXOR_W(inst); break; 
                case AMOAND_W  :  exec_AMOAND_W(inst); break; 
                case AMOOR_W   :  exec_AMOOR_W(inst); break; 
                case AMOMIN_W  :  exec_AMOMIN_W(inst); break; 
                case AMOMAX_W  :  exec_AMOMAX_W(inst); break; 
                case AMOMINU_W :  exec_AMOMINU_W(inst); break; 
                case AMOMAXU_W :  exec_AMOMAXU_W(inst); break; 
                default:
                    fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
                    return 0;
            } break;

        case 0x00:
            return 0;

        default:
            fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct3:0x%x\n", opcode, funct3, funct7);
            return 0;
            /*exit(1);*/
    }
}