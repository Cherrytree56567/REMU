#pragma once
#define MEMORY_BASE 0x80000000
#define MEMORY_SIZE 1024*1024*128

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

#define MIP_SSIP   1 << 1 // MIP Fields
#define MIP_MSIP   1 << 3 // MIP Fields
#define MIP_STIP   1 << 5 // MIP Fields
#define MIP_MTIP   1 << 7 // MIP Fields
#define MIP_SEIP   1 << 9 // MIP Fields
#define MIP_MEIP  1 << 11 // MIP Fields

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

#include <cstdint>
#include <vector>
#include <functional>
#include <iostream>
#include <cstring>
#include <variant>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cinttypes>
#include <chrono>
#include <string>
#include <format>
#include <map>
#include "RAM.h"
#include "OpcodeHelpers.h"
#include "Trap.h"
#include "Bus.h"

enum Mode {
    User = 0b00,
    Supervisor = 0b01,
    Machine = 0b11
};

enum AccessType {
    Instruction,
    Load,
    Store
};

extern "C" uint64_t add(uint64_t a, uint64_t b);

class CPU {
public:
    CPU(std::shared_ptr<Bus> BUS);

    std::variant<uint64_t, Exception> MemoryLoad(uint64_t addr, uint64_t size);
    std::variant<uint64_t, Exception> MemoryStore(uint64_t addr, uint64_t size, uint64_t value);

    uint64_t HandleException(std::variant<uint64_t, Exception> s);

    void update_paging(size_t csr_addr);
    std::variant<uint64_t, Exception> translate(uint64_t addr, AccessType access_type);

    void EnableDebugging(bool t) { EnableDebug = t; }

    uint64_t csrRead(uint64_t csr);
    void csrWrite(uint64_t csr, uint64_t value);

    std::variant<uint64_t, Exception> Fetch();
    std::variant<uint64_t, Exception> Execute(uint64_t inst);
    void DumpRegisters();

    Interrupt check_pending_interrupt();

    void adcp(std::any cpp);

    bool Loop();

    void exec_LUI(uint64_t inst);
    void exec_AUIPC(uint64_t inst);
    void exec_JAL(uint64_t inst);
    void exec_JALR(uint64_t inst);
    void exec_BEQ(uint64_t inst);
    void exec_BNE(uint64_t inst);
    void exec_BLT(uint64_t inst);
    void exec_BGE(uint64_t inst);
    void exec_BLTU(uint64_t inst);
    void exec_BGEU(uint64_t inst);
    void exec_LB(uint64_t inst);
    void exec_LH(uint64_t inst);
    void exec_LW(uint64_t inst);
    void exec_LD(uint64_t inst);
    void exec_LBU(uint64_t inst);
    void exec_LHU(uint64_t inst);
    void exec_LWU(uint64_t inst);
    void exec_SB(uint64_t inst);
    void exec_SH(uint64_t inst);
    void exec_SW(uint64_t inst);
    void exec_SD(uint64_t inst);
    void exec_ADDI(uint64_t inst);
    void exec_SLLI(uint64_t inst);
    void exec_SLTI(uint64_t inst);
    void exec_SLTIU(uint64_t inst);
    void exec_XORI(uint64_t inst);
    void exec_SRLI(uint64_t inst);
    void exec_SRAI(uint64_t inst);
    void exec_ORI(uint64_t inst);
    void exec_ANDI(uint64_t inst);
    void exec_ADD(uint64_t inst);
    void exec_SUB(uint64_t inst);
    void exec_SLL(uint64_t inst);
    void exec_SLT(uint64_t inst);
    void exec_SLTU(uint64_t inst);
    void exec_XOR(uint64_t inst);
    void exec_SRL(uint64_t inst);
    void exec_SRA(uint64_t inst);
    void exec_OR(uint64_t inst);
    void exec_AND(uint64_t inst);
    void exec_FENCE(uint64_t inst);
    std::variant<uint64_t, Exception> exec_ECALL(uint64_t inst);
    std::variant<uint64_t, Exception> exec_EBREAK(uint64_t inst);
    void exec_ADDIW(uint64_t inst);
    void exec_SLLIW(uint64_t inst);
    void exec_SRLIW(uint64_t inst);
    void exec_SRAIW(uint64_t inst);
    void exec_ADDW(uint64_t inst);
    void exec_MULW(uint64_t inst);
    void exec_SUBW(uint64_t inst);
    void exec_DIVW(uint64_t inst);
    void exec_SLLW(uint64_t inst);
    void exec_SRLW(uint64_t inst);
    void exec_DIVUW(uint64_t inst);
    void exec_SRAW(uint64_t inst);
    void exec_REMW(uint64_t inst);
    void exec_REMUW(uint64_t inst);
    void exec_LR_W(uint64_t inst);
    void exec_SC_W(uint64_t inst);
    void exec_AMOSWAP_W(uint64_t inst);
    void exec_AMOADD_W(uint64_t inst);
    void exec_AMOXOR_W(uint64_t inst);
    void exec_AMOAND_W(uint64_t inst);
    void exec_AMOOR_W(uint64_t inst);
    void exec_AMOMIN_W(uint64_t inst);
    void exec_AMOMAX_W(uint64_t inst);
    void exec_AMOMINU_W(uint64_t inst);
    void exec_AMOMAXU_W(uint64_t inst);
    void exec_LR_D(uint64_t inst);
    void exec_SC_D(uint64_t inst);
    void exec_AMOSWAP_D(uint64_t inst);
    void exec_AMOADD_D(uint64_t inst);
    void exec_AMOXOR_D(uint64_t inst);
    void exec_AMOAND_D(uint64_t inst);
    void exec_AMOOR_D(uint64_t inst);
    void exec_AMOMIN_D(uint64_t inst);
    void exec_AMOMAX_D(uint64_t inst);
    void exec_AMOMINU_D(uint64_t inst);
    void exec_AMOMAXU_D(uint64_t inst);
    void exec_CSRRW(uint64_t inst);
    void exec_CSRRS(uint64_t inst);
    void exec_CSRRC(uint64_t inst);
    void exec_CSRRWI(uint64_t inst);
    void exec_CSRRSI(uint64_t inst);
    void exec_CSRRCI(uint64_t inst);
    void exec_SFENCE_VMA(uint64_t inst);
    void exec_SRET(uint64_t inst);
    void exec_MUL(uint64_t inst);
    void exec_MULH(uint64_t inst);
    void exec_MULHSU(uint64_t inst);
    void exec_MULHU(uint64_t inst);
    void exec_DIV(uint64_t inst);
    void exec_DIVU(uint64_t inst);
    void exec_REM(uint64_t inst);
    void exec_REMU(uint64_t inst);
    void exec_MRET(uint64_t inst);

    template<typename... Args>
    void debug(const std::string& format_str, Args&&... args) {
        if constexpr (sizeof...(args) > 0) {
            std::string formatted_str = std::vformat(format_str, std::make_format_args(args...));
            std::cout << "[REMU::CORE] Warning: " << formatted_str << std::endl;
        }
        else if (!format_str.empty()) {
            std::cout << "[REMU::CORE] Warning: " << format_str << std::endl;
        }
        else {
            std::cout << "[REMU::CORE] Warning: " << std::endl;
        }
    }

    std::shared_ptr<Bus> bus;
    std::shared_ptr<RAM> Memory;
    std::any cp;
    uint64_t Registers[32]; // 32 64-bit Registers
    uint64_t ProgramCounter; // Program Counter
    uint64_t CSRegisters[4096]; // Control and Status Registers
    Mode CurrentMode = Mode::Machine;
    bool EnableDebug = false;
    uint64_t PAGE_SIZE = 4096;
    bool enable_paging = false;
    uint64_t page_table = 0;
    uint64_t cycle_counter;
    std::chrono::steady_clock::time_point start_time;
    std::vector<uint64_t> s;
    uint64_t ss = 0;
};