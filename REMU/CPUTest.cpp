#define CPU_NAME CPUTest
#include "CPUTest.h"

CPU_NAME::CPU_NAME(std::shared_ptr<Bus> BUS) : CPU(BUS) {
    Registers[0] = 0x00; // Register x0 hardwired to 0
    Registers[2] = MEMORY_BASE + MEMORY_SIZE; // Set Stack Pointer
    ProgramCounter = MEMORY_BASE;
    bus = BUS;
    Memory = bus->GetRam();
}

std::variant<uint64_t, Exception> CPU_NAME::Fetch() {
    return MemoryLoad(ProgramCounter, 32);
}

void CPU_NAME::debug(std::string s) {
    std::cout << s;
}

bool CPU_NAME::Loop() {
    uint32_t inst;
    try {
        inst = std::get<uint64_t>(Fetch());
    }
    catch (const std::bad_variant_access& e) {
        return false;
    }

    ProgramCounter += 4;

    try {
        auto result = Execute(inst);
        if (std::holds_alternative<uint64_t>(result) && std::get<uint64_t>(result) != 0) {
            return false;
        }
    }
    catch (const std::bad_variant_access& e) {
        return false;
    }

    if (ProgramCounter == 0)
        return false;

    return true;
}

std::variant<uint64_t, Exception> CPU_NAME::MemoryLoad(uint64_t addr, uint64_t size) {
    switch (size) {
    case 8:  return Memory->MemoryLoad8(addr);  break;
    case 16: return Memory->MemoryLoad16(addr); break;
    case 32: return Memory->MemoryLoad32(addr); break;
    case 64: return Memory->MemoryLoad64(addr); break;
    default: break;
    }
    return Exception::LoadAccessFault;
}

std::variant<uint64_t, Exception> CPU_NAME::MemoryStore(uint64_t addr, uint64_t size, uint64_t value) {
    switch (size) {
    case 8:  Memory->MemoryStore8(addr, value);  break;
    case 16: Memory->MemoryStore16(addr, value); break;
    case 32: Memory->MemoryStore32(addr, value); break;
    case 64: Memory->MemoryStore64(addr, value); break;
    default: return Exception::StoreAMOAccessFault;
    }
    return (uint64_t)0;
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

std::variant<uint64_t, Exception> CPU_NAME::Execute(uint32_t inst) {
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
                default: ;
            } break;

        case LOAD:
            switch (funct3) {
                case LB  :  exec_LB(inst); break;  // RV32I Base
                case LH  :  exec_LH(inst); break;  // RV32I Base 
                case LW  :  exec_LW(inst); break;  // RV32I Base  
                case LD  :  exec_LD(inst); break;  // RV64I Base
                case LBU :  exec_LBU(inst); break;  // RV32I Base 
                case LHU :  exec_LHU(inst); break;  // RV32I Base 
                case LWU :  exec_LWU(inst); break;  // RV64I Base
                default: ;
            } break;

        case S_TYPE:
            switch (funct3) {
                case SB  :  exec_SB(inst); break;  // RV32I Base  
                case SH  :  exec_SH(inst); break;  // RV32I Base  
                case SW  :  exec_SW(inst); break;  // RV32I Base  
                case SD  :  exec_SD(inst); break;  // RV64I Base
                default: ;
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
                        default: ;
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
                        case ADD: exec_ADD(inst); // RV32I Base
                        case SUB: exec_ADD(inst); // RV32I Base
                        default: ;
                    } break;
                case SLL:  exec_SLL(inst); break; // RV32I Base
                case SLT:  exec_SLT(inst); break; // RV32I Base
                case SLTU: exec_SLTU(inst); break; // RV32I Base
                case XOR:  exec_XOR(inst); break; // RV32I Base
                case SR:   
                    switch (funct7) {
                        case SRL:  exec_SRL(inst); break; // RV32I Base
                        case SRA:  exec_SRA(inst); break; // RV32I Base
                        default: ;
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
                case SRIW : 
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
                default: ;
            } break;

        case CSR:
            switch (funct3) {
                case ECALLBREAK: return exec_ECALLBREAK(inst); break; // RV32I Base
                case CSRRW  :  exec_CSRRW(inst); break; // RV64I Base
                case CSRRS  :  exec_CSRRS(inst); break; // RV64I Base
                case CSRRC  :  exec_CSRRC(inst); break; // RV64I Base
                case CSRRWI :  exec_CSRRWI(inst); break; // RV64I Base
                case CSRRSI :  exec_CSRRSI(inst); break; // RV64I Base
                case CSRRCI :  exec_CSRRCI(inst); break; // RV64I Base
                default:
                    fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
                    return Exception::IllegalInstruction;
            } break;

        case AMO_W:
            switch (funct7 >> 2) { // since, funct[1:0] = aq, rl
                case LR_W      :  exec_LR_W(inst); break; // RV32A Standard
                case SC_W      :  exec_SC_W(inst); break; // RV32A Standard
                case AMOSWAP_W :  exec_AMOSWAP_W(inst); break; // RV32A Standard 
                case AMOADD_W  :  exec_AMOADD_W(inst); break; // RV32A Standard
                case AMOXOR_W  :  exec_AMOXOR_W(inst); break; // RV32A Standard
                case AMOAND_W  :  exec_AMOAND_W(inst); break; // RV32A Standard
                case AMOOR_W   :  exec_AMOOR_W(inst); break; // RV32A Standard
                case AMOMIN_W  :  exec_AMOMIN_W(inst); break; // RV32A Standard
                case AMOMAX_W  :  exec_AMOMAX_W(inst); break; // RV32A Standard
                case AMOMINU_W :  exec_AMOMINU_W(inst); break; // RV32A Standard
                case AMOMAXU_W :  exec_AMOMAXU_W(inst); break; // RV32A Standard
                default:
                    fprintf(stderr, "[-] ERROR-> opcode:0x%x, funct3:0x%x, funct7:0x%x\n", opcode, funct3, funct7);
                    return Exception::IllegalInstruction;
            } break;

        case AMO_D:
            switch (funct7 >> 2) { // since, funct[1:0] = aq, rl
                case LR_D      :  exec_LR_D(inst); break; // RV64A Standard
                case SC_D      :  exec_SC_D(inst); break; // RV64A Standard
                case AMOSWAP_D :  exec_AMOSWAP_D(inst); break; // RV64 Standard 
                case AMOADD_D  :  exec_AMOADD_D(inst); break; // RV64A Standard
                case AMOXOR_D  :  exec_AMOXOR_D(inst); break; // RV64A Standard
                case AMOAND_D  :  exec_AMOAND_D(inst); break; // RV64A Standard
                case AMOOR_D  :  exec_AMOOR_D(inst); break; // RV64A Standard
                case AMOMIN_D  :  exec_AMOMIN_D(inst); break; // RV64A Standard
                case AMOMAX_D  :  exec_AMOMAX_D(inst); break; // RV64A Standard
                case AMOMINU_D :  exec_AMOMINU_D(inst); break; // RV64A Standard
                case AMOMAXU_D :  exec_AMOMAXU_D(inst); break; // RV64A Standard
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
}