# SAP-1 emulator core (Xtensa assembly)
# Problem 31: 14 + 27 - 9
# Opcodes: LDA=0x0  ADD=0x1  SUB=0x2  OUT=0xE  HLT=0xF

    .data
    .align 4
    .global sap1_A, sap1_B, sap1_OUT, sap1_PC, sap1_IR, sap1_HALT, sap1_mem
sap1_A:    .word 0
sap1_B:    .word 0
sap1_OUT:  .word 0
sap1_PC:   .word 0
sap1_IR:   .word 0
sap1_HALT: .word 0

sap1_mem:
    .byte 0x0D          # 0x0: LDA 0xD
    .byte 0x1E          # 0x1: ADD 0xE
    .byte 0x2F          # 0x2: SUB 0xF
    .byte 0xE0          # 0x3: OUT
    .byte 0xF0          # 0x4: HLT
    .space 8            # 0x5 - 0xC: unused
    .byte 14            # 0xD: data
    .byte 27            # 0xE: data
    .byte 9             # 0xF: data

    .text
    .align 4
    .global sap1_step
    .type sap1_step, @function
sap1_step:
    entry   a1, 16

    movi    a3, sap1_HALT
    l32i    a4, a3, 0
    bnez    a4, .Lhalted

    # FETCH
    movi    a3, sap1_PC
    l32i    a4, a3, 0
    movi    a5, sap1_mem
    add     a6, a5, a4
    l8ui    a7, a6, 0
    addi    a4, a4, 1
    s32i    a4, a3, 0
    movi    a8, sap1_IR
    s32i    a7, a8, 0

    # DECODE
    srli    a8, a7, 4
    movi    a9, 0x0F
    and     a9, a7, a9
    add     a10, a5, a9
    l8ui    a10, a10, 0
    movi    a11, sap1_A
    l32i    a3, a11, 0

    # EXECUTE
    beqz    a8, .Llda
    movi    a4, 1
    beq     a8, a4, .Ladd
    movi    a4, 2
    beq     a8, a4, .Lsub
    movi    a4, 0xE
    beq     a8, a4, .Lout
    j       .Lhlt

.Llda:
    s32i    a10, a11, 0
    j       .Lend
.Ladd:
    movi    a4, sap1_B
    s32i    a10, a4, 0
    add     a3, a3, a10
    s32i    a3, a11, 0
    j       .Lend
.Lsub:
    movi    a4, sap1_B
    s32i    a10, a4, 0
    sub     a3, a3, a10
    s32i    a3, a11, 0
    j       .Lend
.Lout:
    movi    a4, sap1_OUT
    s32i    a3, a4, 0
    j       .Lend
.Lhlt:
    movi    a4, 1
    movi    a5, sap1_HALT
    s32i    a4, a5, 0
.Lend:
    movi    a2, 1
    retw.n
.Lhalted:
    movi    a2, 0
    retw.n