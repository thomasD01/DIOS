#pragma once

#include <stdint.h>

struct IDTEntry {
    uint16_t offset_low;    // Lower 16 bits of handler function address
    uint16_t selector;      // Kernel segment selector
    uint8_t  ist;           // Interrupt Stack Table offset
    uint8_t  type_attr;     // Type and attributes
    uint16_t offset_mid;    // Middle 16 bits of handler function address
    uint32_t offset_high;   // Higher 32 bits of handler function address
    uint32_t zero;          // Reserved
} __attribute__((packed));

struct IDTPointer {
    uint16_t limit;         // Size of the IDT
    uint64_t base;          // Base address of the IDT
} __attribute__((packed));

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss; // Pushed automatically by CPU
} Registers;

void idt_init(void);
void idt_set_gate(int vector, void* isr);
void exception_handler(Registers* regs);

// Assembly function to load the IDT
extern void idt_load(struct IDTPointer* ptr);

// ISR handlers (defined in assembly)
extern void isr0();
extern void isr6();
extern void isr8();
extern void isr13();
extern void isr14();
