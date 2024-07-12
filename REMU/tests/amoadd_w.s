# See LICENSE for license details.

#*****************************************************************************
# amoadd_w.S
#-----------------------------------------------------------------------------
#
# Test amoadd.w instruction.
#

#include "riscv_test.h"
#include "test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  TEST_CASE(2, a4, 0xffffffff80000000, \
    li a0, 0xffffffff80000000; \
    li a1, 0xfffffffffffff800; \
    la a3, amo_operand; \
    sw a0, 0(a3); \
    amoadd.w	a4, a1, 0(a3); \
  )

  TEST_CASE(3, a5, 0x000000007ffff800, lw a5, 0(a3))

  # try again after a cache miss
  TEST_CASE(4, a4, 0x000000007ffff800, \
    li  a1, 0xffffffff80000000; \
    amoadd.w a4, a1, 0(a3); \
  )

  TEST_CASE(5, a5, 0xfffffffffffff800, lw a5, 0(a3))

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

RVTEST_DATA_END

    .bss
    .align 3
amo_operand:
    .dword 0
