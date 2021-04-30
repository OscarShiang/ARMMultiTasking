#include "port/sysregs.h"

.section ".text.boot"

.globl _Reset
_Reset:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0

    // set stack before our code
    ldr     x1, =stack_top

    // set up EL1
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits

    // running at EL3?
    cmp     x0, #12
    bne     5f
    // should never be executed, just for completeness
    mov     x2, #0x5b1
    msr     scr_el3, x2
    mov     x2, #0x3c9
    msr     spsr_el3, x2
    adr     x2, 5f
    msr     elr_el3, x2
    eret

    // running at EL2?
5:  cmp     x0, #4
    beq     5f
    msr     sp_el1, x1
    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // Setup SCTLR access
    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2
    // set up exception handlers
    ldr     x2, =_vectors
    msr     vbar_el1, x2
    // change execution level to EL1
    mov     x2, #0x3c4
    msr     spsr_el2, x2
    adr     x2, 5f
    msr     elr_el2, x2
    eret

5:  ldr     x1, =stack_top
    mov     sp, x1

    mov     x1, #(0x3 << 20)
    msr     cpacr_el1, x1
    isb

    // jump to C code, should not return
    bl      entry
    // for failsafe, halt this core too
    b       1b

    .macro ventry label
    .align 7
    b \label
    .endm

.align 11
_vectors:
    ventry	handle_svc // Synchronous EL1t
    ventry	invalid_entry // IRQ EL1t
    ventry	invalid_entry // FIQ EL1t
    ventry	invalid_entry // Error EL1t

    ventry	handle_svc // Synchronous EL1h
    ventry	invalid_entry // IRQ EL1h
    ventry	invalid_entry // FIQ EL1h
    ventry	invalid_entry // Error EL1h

    ventry	handle_svc // Synchronous 64-bit EL0
    ventry	invalid_entry // IRQ 64-bit EL0
    ventry	invalid_entry // FIQ 64-bit EL0
    ventry	invalid_entry // Error 64-bit EL0

    ventry	invalid_entry // Synchronous 32-bit EL0
    ventry	invalid_entry // IRQ 32-bit EL0
    ventry	invalid_entry // FIQ 32-bit EL0
    ventry	invalid_entry // Error 32-bit EL0
