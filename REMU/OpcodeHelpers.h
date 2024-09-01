#pragma once

uint64_t rd(uint64_t inst);
uint64_t rs1(uint64_t inst);
uint64_t rs2(uint64_t inst);
uint64_t imm_I(uint64_t inst);
uint64_t imm_S(uint64_t inst);
uint64_t imm_U(uint64_t inst);
uint64_t imm_J(uint64_t inst);
uint64_t imm_B(uint64_t inst);
uint32_t shamt(uint64_t inst);
uint64_t csr(uint64_t inst);
uint64_t wrapping_shr(uint64_t value, unsigned int shift);
uint64_t wrapping_shl(uint64_t value, unsigned int shift);