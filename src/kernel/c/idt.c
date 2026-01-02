#include <stdbool.h>

#include "idt.h"
#include "x86.h"
#include "stdio.h"
#include "graphics.h"

// Define the table (256 entries)
struct IDTEntry idt[256];
struct IDTPointer idtr;

void idt_set_gate(int n, void* handler) {
  uint64_t addr = (uint64_t)handler;
  idt[n].offset_low  = addr & 0xFFFF;
  idt[n].offset_mid  = (addr >> 16) & 0xFFFF;
  idt[n].offset_high = (addr >> 32) & 0xFFFFFFFF;
  
  idt[n].selector = 0x38; // KERNEL_CODE_SEGMENT (Check your specific GDT/Bootloader!)
                          // If you use GNU-EFI, it might be 0x38 or 0x28. 
                          // Check "CS" register in qemu 'info registers' to be sure.
                          
  idt[n].ist = 0;
  idt[n].type_attr = 0x8E;
  idt[n].zero = 0;
}

void idt_init() {
  idt_set_gate(0, isr0);
  idt_set_gate(6, isr6);
  idt_set_gate(8, isr8);
  idt_set_gate(13, isr13);
  idt_set_gate(14, isr14);

  idtr.base = (uint64_t)&idt;
  idtr.limit = sizeof(struct IDTEntry) * 256 - 1;

  idt_load(&idtr);
  x86_sti();
}

void exception_handler(Registers* regs) {
  kclear_screen(0x0000008B);
  printf("!!! KERNEL PANIC !!!\n");
  printf("Interrupt: %d  Error Code: %x\n", regs->int_no, regs->err_code);
  printf("RIP: %lx\n", regs->rip);

  while (1)
  {
    x86_halt();
  }
}