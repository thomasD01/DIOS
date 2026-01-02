.global _start
.extern kernel_main

.section .text
.code64

_start:
    mov $stack_top, %rsp
    call kernel_main
    cli
hang:
    hlt
    jmp hang

.section .bss
.align 16
stack_bottom:
    .space 0x4000
stack_top:
