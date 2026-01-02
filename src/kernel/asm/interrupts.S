.code64
.section .text

/* ISR stub for interrupts without error code */
.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    cli
    push $0
    push $\num
    jmp isr_common_stub
.endm

/* ISR stub for interrupts with error code */
.macro ISR_ERRCODE num
.global isr\num
isr\num:
    cli
    push $\num
    jmp isr_common_stub
.endm

/* Define the handlers for the first few critical exceptions */
ISR_NOERRCODE 0   /* Divide by Zero */
ISR_NOERRCODE 6   /* Invalid Opcode */
ISR_ERRCODE   8   /* Double Fault */
ISR_ERRCODE   13  /* General Protection Fault */
ISR_ERRCODE   14  /* Page Fault */

/* Common ISR handler stub */
.extern exception_handler
isr_common_stub:
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push %rdi
    push %rsi
    push %rbp
    push %rdx
    push %rcx
    push %rbx
    push %rax

    mov %rsp, %rdi
    call exception_handler

    pop %rax
    add $16, %rsp
    iretq

/* -----------------------------------------
   Function: void idt_load(struct IDTR* ptr)
   ABI: RDI = ptr
   ----------------------------------------- */
.global idt_load
idt_load:
    lidt (%rdi)    /* Load the IDT pointer from the address in RDI */
    ret