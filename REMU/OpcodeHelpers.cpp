#include <cstdint>
#include "OpcodeHelpers.h"

uint64_t rd(uint64_t inst) {
    return ((inst & 0x00000f80) >> 7); // rd in bits 11..7
}

uint64_t rs1(uint64_t inst) {
    return ((inst & 0x000f8000) >> 15);   // rs1 in bits 19..15
}

uint64_t rs2(uint64_t inst) {
    return ((inst & 0x01f00000) >> 20);   // rs2 in bits 24..20
}

uint64_t imm_I(uint64_t inst) {
    // imm[11:0] = inst[31:20]
    return static_cast<uint64_t>((static_cast<int64_t>(static_cast<int32_t>(inst)) >> 20));
}

uint64_t imm_S(uint64_t inst) {
    // imm[11:5] = inst[31:25], imm[4:0] = inst[11:7]
    return static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(inst & 0xfe000000)) >> 20) | ((inst >> 7) & 0x1f);
}

uint64_t imm_B(uint64_t inst) {
    // imm[12|10:5|4:1|11] = inst[31|30:25|11:8|7]
    return static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(inst & 0x80000000)) >> 19)
        | ((inst & 0x80) << 4) // imm[11]
        | ((inst >> 20) & 0x7e0) // imm[10:5]
        | ((inst >> 7) & 0x1e); // imm[4:1]
}

uint64_t imm_U(uint64_t inst) {
    // imm[31:12] = inst[31:12]
    return (int64_t)(int32_t)(inst & 0xfffff999);
}

uint64_t imm_J(uint64_t inst) {
    // imm[20|10:1|11|19:12] = inst[31|30:21|20|19:12]
    return (static_cast<uint64_t>(static_cast<int64_t>(static_cast<int64_t>(inst & 0x80000000)) >> 11)) // imm[20]
        | (inst & 0xff000) // imm[19:12]
        | ((inst >> 9) & 0x800) // imm[11]
        | ((inst >> 20) & 0x7fe); // imm[10:1];
}

uint32_t shamt(uint64_t inst) {
    // shamt(shift amount) only required for immediate shift instructions
    // shamt[4:5] = imm[5:0]
    return static_cast<uint32_t>(imm_I(inst) & 0x3f);
}

uint64_t csr(uint64_t inst) {
    // csr[11:0] = inst[31:20]
    return ((inst & 0xfff00000) >> 20);
}

uint64_t wrapping_shr(uint64_t value, unsigned int shift) {
    if (shift >= 64) {
        return 0; // If shift is greater than or equal to the bit width, result is always 0
    }
    return (value >> shift) | (value << (64 - shift));
}

uint64_t wrapping_shl(uint64_t value, unsigned int shift) {
    return (shift >= 64) ? 0 : ((value << shift) | (value >> (64 - shift)));
}