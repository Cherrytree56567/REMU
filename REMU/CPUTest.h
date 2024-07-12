#pragma once
#define MEMORY_BASE 0x80000000
#define MEMORY_SIZE 1024*1024*1

#define USTATUS     0x000 // URW User status register.
#define UIE         0x004 // URW User interrupt-enable register.
#define UTVEC       0x005 // URW User trap handler base address.

//User Trap Handling
#define USCRATCH    0x040 // URW Scratch register for user trap handlers.
#define UEPC        0x041 // URW User exception program counter.
#define UCAUSE      0x042 // URW User trap cause.
#define UTVAL       0x043 // URW User bad address or instruction.
#define UIP         0x044 // URW User interrupt pending.

//User Floating-Point CSRs
#define FFLAGS      0x001 // URW Floating-Point Accrued Exceptions.
#define FRM         0x002 // URW Floating-Point Dynamic Rounding Mode.
#define FCSR        0x003 // URW Floating-Point Control and Status Register (frm + fflags)

//User Counter/Timers
#define CYCLE       0xC00 // URO Cycle counter for RDCYCLE instruction.
#define TIME        0xC01 // URO Timer for RDTIME instruction.
#define INSTRET     0xC02 // URO Instructions-retired counter for RDINSTRET instruction.
#define HPMCOUNTER3 0xC03 // URO Performance-monitoring counter.
#define HPMCOUNTER4 0xC04 // URO Performance-monitoring counter.
// ... hpm counter 4-31 (TODO)
#define HPMCOUNTER31 0xC1F // URO Performance-monitoring counter.
#define CYCLEH      0xC80  // URO Upper 32 bits of cycle, RV32I only.
#define TIMEH       0xC81  // URO Upper 32 bits of time, RV32I only.
#define INSTRETH    0xC82  // URO Upper 32 bits of instret, RV32I only.
#define HPMCOUNTER3H 0xC83 // URO Upper 32 bits of hpmcounter3, RV32I only.
#define HPMCOUNTER4H 0xC84 // URO Upper 32 bits of hpmcounter4, RV32I only.
// ... hpm counter 4-31 (TODO)
#define HPMCOUNTER31H 0xC9F URO 


//Supervisor Trap Setup
#define SSTATUS     0x100 // SRW Supervisor status register.
#define SEDELEG     0x102 // SRW Supervisor exception delegation register.
#define SIDELEG     0x103 // SRW Supervisor interrupt delegation register.
#define SIE         0x104 // SRW Supervisor interrupt-enable register.
#define STVEC       0x105 // SRW Supervisor trap handler base address.
#define SCOUNTEREN  0x106 // SRW Supervisor counter enable.

//Supervisor Trap Handling
#define SSCRATCH    0x140 // SRW Scratch register for supervisor trap handlers.
#define SEPC        0x141 // SRW Supervisor exception program counter.
#define SCAUSE      0x142 // SRW Supervisor trap cause.
#define STVAL       0x143 // SRW Supervisor bad address or instruction.
#define SIP         0x144 // SRW Supervisor interrupt pending.

//Supervisor Protection and Translation
#define SATP        0x180 // SRW Supervisor address translation and protection.

//Machine Information Registers
#define MVENDORID   0xF11 // MRO Vendor ID.
#define MARCHID     0xF12 // MRO Architecture ID.
#define MIMPID      0xF13 // MRO Implementation ID.
#define MHARTID     0xF14 // MRO Hardware thread ID.

//Machine Trap Setup
#define MSTATUS     0x300 // MRW Machine status register.
#define MISA        0x301 // MRW ISA and extensions
#define MEDELEG     0x302 // MRW Machine exception delegation register.
#define MIDELEG     0x303 // MRW Machine interrupt delegation register.
#define MIE         0x304 // MRW Machine interrupt-enable register.
#define MTVEC       0x305 // MRW Machine trap-handler base address.
#define MCOUNTEREN  0x306 // MRW Machine counter enable.

//Machine Trap Handling
#define MSCRATCH    0x340 // MRW Scratch register for machine trap handlers.
#define MEPC        0x341 // MRW Machine exception program counter.
#define MCAUSE      0x342 // MRW Machine trap cause.
#define MTVAL       0x343 // MRW Machine bad address or instruction.
#define MIP         0x344 // MRW Machine interrupt pending.

//Machine Memory Protection
#define PMPCFG0     0x3A0 // MRW Physical memory protection configuration.
#define PMPCFG1     0x3A1 // MRW Physical memory protection configuration, RV32 only.
#define PMPCFG2     0x3A2 // MRW Physical memory protection configuration.
#define PMPCFG3     0x3A3 // MRW Physical memory protection configuration, RV32 only.
#define PMPADDR0    0x3B0 // MRW Physical memory protection address register.
#define PMPADDR1    0x3B1 // MRW Physical memory protection address register.
// ... 2-15 /TODO
#define PMPADDR15   0x3BF // MRW Physical memory protection address register.

//Machine Counter/Timers
#define MCYCLE       0xB00 // MRW Machine cycle counter.
#define MINSTRET     0xB02 // MRW Machine instructions-retired counter.
#define MHPMCOUNTER3 0xB03 // MRW Machine performance-monitoring counter.
#define MHPMCOUNTER4 0xB04 // MRW Machine performance-monitoring counter.
// #define ...
#define MHPMCOUNTER31 0xB1F // MRW Machine performance-monitoring counter.
#define MCYCLEH       0xB80 // MRW Upper 32 bits of mcycle, RV32I only.
#define MINSTRETH     0xB82 // MRW Upper 32 bits of minstret, RV32I only.
#define MHPMCOUNTER3H 0xB83 // MRW Upper 32 bits of mhpmcounter3, RV32I only.
#define MHPMCOUNTER4H 0xB84 // MRW Upper 32 bits of mhpmcounter4, RV32I only.
// ...
#define MHPMCOUNTER31H 0xB9F // MRW Upper 32 bits of mhpmcounter31, RV32I only.

//Machine Counter Setup
#define MCOUNTINHIBIT 0x320 // MRW Machine counter-inhibit register.
#define MHPMEVENT3    0x323 // MRW Machine performance-monitoring event selector.
#define MHPMEVENT4    0x324 // MRW Machine performance-monitoring event selector.
//#define ...
#define MHPMEVENT31 0x33F // MRW Machine performance-monitoring event selector.

//Debug/Trace Registers (shared with Debug Mode)
#define TSELECT     0x7A0 // MRW Debug/Trace trigger register select.
#define TDATA1      0x7A1 // MRW First Debug/Trace trigger data register.
#define TDATA2      0x7A2 // MRW Second Debug/Trace trigger data register.
#define TDATA3      0x7A3 // MRW Third Debug/Trace trigger data register.

//Debug Mode Registers
#define DCSR        0x7B0 // DRW Debug control and status register.
#define DPC         0x7B1 // DRW Debug PC.
#define DSCRATCH0   0x7B2 // DRW Debug scratch register 0.
#define DSCRATCH1   0x7B3 // DRW Debug scratch register 1.

#define LUI     0x37 
#define AUIPC   0x17 

#define JAL     0x6f 
#define JALR    0x67 

#define B_TYPE  0x63
    #define BEQ     0x0
    #define BNE     0x1
    #define BLT     0x4
    #define BGE     0x5
    #define BLTU    0x6
    #define BGEU    0x7

#define LOAD    0x03
    #define LB      0x0
    #define LH      0x1
    #define LW      0x2
    #define LD      0x3
    #define LBU     0x4
    #define LHU     0x5
    #define LWU     0x6

#define S_TYPE  0x23
    #define SB      0x0
    #define SH      0x1
    #define SW      0x2
    #define SD      0x3

#define I_TYPE  0x13
    #define ADDI    0x0
    #define SLLI    0x1
    #define SLTI    0x2
    #define SLTIU   0x3
    #define XORI    0x4
    #define SRI     0x5
        #define SRLI    0x00
        #define SRAI    0x20
    #define ORI     0x6
    #define ANDI    0x7

#define R_TYPE  0x33
    #define ADDSUB  0x0
        #define ADD     0x00
        #define SUB     0x20
    #define SLL     0x1
    #define SLT     0x2
    #define SLTU    0x3
    #define XOR     0x4
    #define SR      0x5
        #define SRL     0x00
        #define SRA     0x20
    #define OR      0x6
    #define AND     0x7

#define FENCE   0x0f

#define I_TYPE_64 0x1b
    #define ADDIW   0x0
    #define SLLIW   0x1
    #define SRIW    0x5
        #define SRLIW   0x00
        #define SRAIW   0x20

#define R_TYPE_64 0x3b
    #define ADDSUB   0x0
        #define ADDW    0x00
        #define MULW    0x01
        #define SUBW    0x20
    #define DIVW    0x4
    #define SLLW    0x1
    #define SRW     0x5
        #define SRLW   0x00
        #define DIVUW   0x01
        #define SRAW   0x20
    #define REMW    0x6
    #define REMUW   0x7

#define CSR 0x73
    #define ECALLBREAK    0x00     // contains both ECALL and EBREAK
    #define CSRRW   0x01
    #define CSRRS   0x02
    #define CSRRC   0x03
    #define CSRRWI  0x05
    #define CSRRSI  0x06
    #define CSRRCI  0x07

#define AMO_W 0x2f
    #define LR_W        0x02
    #define SC_W        0x03
    #define AMOSWAP_W   0x01
    #define AMOADD_W    0x00
    #define AMOXOR_W    0x04
    #define AMOAND_W    0x0c
    #define AMOOR_W     0x08
    #define AMOMIN_W    0x10
    #define AMOMAX_W    0x14
    #define AMOMINU_W   0x18
    #define AMOMAXU_W   0x1c

#define AMO_D 0x3f
    #define LR_D        0x02
    #define SC_D        0x03
    #define AMOSWAP_D   0x01
    #define AMOADD_D    0x00
    #define AMOXOR_D    0x04
    #define AMOAND_D    0x0c
    #define AMOOR_D     0x08
    #define AMOMIN_D    0x10
    #define AMOMAX_D    0x14
    #define AMOMINU_D   0x18
    #define AMOMAXU_D   0x1c
#include <cstdint>
#include <vector>
#include <functional>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include "CPU.h"
#include "RAM.h"

class CPUTest: public CPU {
public:
    CPUTest(std::shared_ptr<RAM> ram);

    virtual uint64_t MemoryLoad(uint64_t addr, uint64_t size) override;
    virtual void MemoryStore(uint64_t addr, uint64_t size, uint64_t value) override;

    virtual uint64_t csrRead(uint64_t csr) override;
    virtual void csrWrite(uint64_t csr, uint64_t value) override;

    virtual uint32_t Fetch() override;
    virtual int Execute(uint32_t inst) override;
    virtual void DumpRegisters() override;

    virtual bool Loop() override;

private:

    virtual void exec_LUI(uint32_t inst) override;
    virtual void exec_AUIPC(uint32_t inst) override;
    virtual void exec_JAL(uint32_t inst) override;
    virtual void exec_JALR(uint32_t inst) override;
    virtual void exec_BEQ(uint32_t inst) override;
    virtual void exec_BNE(uint32_t inst) override;
    virtual void exec_BLT(uint32_t inst) override;
    virtual void exec_BGE(uint32_t inst) override;
    virtual void exec_BLTU(uint32_t inst) override;
    virtual void exec_BGEU(uint32_t inst) override;
    virtual void exec_LB(uint32_t inst) override;
    virtual void exec_LH(uint32_t inst) override;
    virtual void exec_LW(uint32_t inst) override;
    virtual void exec_LD(uint32_t inst) override;
    virtual void exec_LBU(uint32_t inst) override;
    virtual void exec_LHU(uint32_t inst) override;
    virtual void exec_LWU(uint32_t inst) override;
    virtual void exec_SB(uint32_t inst) override;
    virtual void exec_SH(uint32_t inst) override;
    virtual void exec_SW(uint32_t inst) override;
    virtual void exec_SD(uint32_t inst) override;
    virtual void exec_ADDI(uint32_t inst) override;
    virtual void exec_SLLI(uint32_t inst) override;
    virtual void exec_SLTI(uint32_t inst) override;
    virtual void exec_SLTIU(uint32_t inst) override;
    virtual void exec_XORI(uint32_t inst) override;
    virtual void exec_SRLI(uint32_t inst) override;
    virtual void exec_SRAI(uint32_t inst) override;
    virtual void exec_ORI(uint32_t inst) override;
    virtual void exec_ANDI(uint32_t inst) override;
    virtual void exec_ADD(uint32_t inst) override;
    virtual void exec_SUB(uint32_t inst) override;
    virtual void exec_SLL(uint32_t inst) override;
    virtual void exec_SLT(uint32_t inst) override;
    virtual void exec_SLTU(uint32_t inst) override;
    virtual void exec_XOR(uint32_t inst) override;
    virtual void exec_SRL(uint32_t inst) override;
    virtual void exec_SRA(uint32_t inst) override;
    virtual void exec_OR(uint32_t inst) override;
    virtual void exec_AND(uint32_t inst) override;
    virtual void exec_FENCE(uint32_t inst) override;
    virtual void exec_ECALL(uint32_t inst) override;
    virtual void exec_EBREAK(uint32_t inst) override;
    virtual void exec_ECALLBREAK(uint32_t inst) override;
    virtual void exec_ADDIW(uint32_t inst) override;
    virtual void exec_SLLIW(uint32_t inst) override;
    virtual void exec_SRLIW(uint32_t inst) override;
    virtual void exec_SRAIW(uint32_t inst) override;
    virtual void exec_ADDW(uint32_t inst) override;
    virtual void exec_MULW(uint32_t inst) override;
    virtual void exec_SUBW(uint32_t inst) override;
    virtual void exec_DIVW(uint32_t inst) override;
    virtual void exec_SLLW(uint32_t inst) override;
    virtual void exec_SRLW(uint32_t inst) override;
    virtual void exec_DIVUW(uint32_t inst) override;
    virtual void exec_SRAW(uint32_t inst) override;
    virtual void exec_REMW(uint32_t inst) override;
    virtual void exec_REMUW(uint32_t inst) override;
    virtual void exec_LR_W(uint32_t inst) override;
    virtual void exec_SC_W(uint32_t inst) override;
    virtual void exec_AMOSWAP_W(uint32_t inst) override;
    virtual void exec_AMOADD_W(uint32_t inst) override;
    virtual void exec_AMOXOR_W(uint32_t inst) override;
    virtual void exec_AMOAND_W(uint32_t inst) override;
    virtual void exec_AMOOR_W(uint32_t inst) override;
    virtual void exec_AMOMIN_W(uint32_t inst) override;
    virtual void exec_AMOMAX_W(uint32_t inst) override;
    virtual void exec_AMOMINU_W(uint32_t inst) override;
    virtual void exec_AMOMAXU_W(uint32_t inst) override;
    virtual void exec_LR_D(uint32_t inst) override;
    virtual void exec_SC_D(uint32_t inst) override;
    virtual void exec_AMOSWAP_D(uint32_t inst) override;
    virtual void exec_AMOADD_D(uint32_t inst) override;
    virtual void exec_AMOXOR_D(uint32_t inst) override;
    virtual void exec_AMOAND_D(uint32_t inst) override;
    virtual void exec_AMOOR_D(uint32_t inst) override;
    virtual void exec_AMOMIN_D(uint32_t inst) override;
    virtual void exec_AMOMAX_D(uint32_t inst) override;
    virtual void exec_AMOMINU_D(uint32_t inst) override;
    virtual void exec_AMOMAXU_D(uint32_t inst) override;
    virtual void exec_CSRRW(uint32_t inst) override;
    virtual void exec_CSRRS(uint32_t inst) override;
    virtual void exec_CSRRC(uint32_t inst) override;
    virtual void exec_CSRRWI(uint32_t inst) override;
    virtual void exec_CSRRSI(uint32_t inst) override;
    virtual void exec_CSRRCI(uint32_t inst) override;

    void debug(std::string s);

    std::shared_ptr<RAM> Memory;
    uint64_t Registers[32]; // 32 64-bit Registers
    uint64_t ProgramCounter; // Program Counter
    uint64_t CSRegisters[4069]; // Control and Status Registers
    Mode CurrentMode = Mode::Machine;
};