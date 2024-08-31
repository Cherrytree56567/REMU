#pragma once

uint64_t rd(uint32_t inst);
uint64_t rs1(uint32_t inst);
uint64_t rs2(uint32_t inst);
uint64_t imm_I(uint32_t inst);
uint64_t imm_S(uint32_t inst);
uint64_t imm_U(uint32_t inst);
uint64_t imm_J(uint32_t inst);
uint64_t imm_B(uint32_t inst);
uint32_t shamt(uint32_t inst);
uint64_t csr(uint32_t inst);
uint64_t wrapping_shr(uint64_t value, unsigned int shift);
uint64_t wrapping_shl(uint64_t value, unsigned int shift);