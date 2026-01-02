#include <stdint.h>
#include "bootinfo.h"
#include "stdio.h"
#include "x86.h"
#include "graphics.h"
#include "idt.h"

void serial_print(const char* s) {
    while (*s) {
        x86_outb(0x3F8, *s++);
    }
}

void serial_print_hex(uint64_t n) {
    char* chars = "0123456789ABCDEF";
    for(int i = 60; i >= 0; i -= 4) {
        x86_outb(0x3F8, chars[(n >> i) & 0xF]);
    }
}

BootInfo* gBootInfo = NULL;

void kernel_main(BootInfo* boot_info) {
    idt_init();

    gBootInfo = boot_info;

    serial_print("Kernel Initialized!\n");

    clrscr();

    kprint_str("Welcome to DIOS Kernel!\n", 0xFFFFFF); // White color
    kprint_str("Graphics Initialized.\n", 0x00FF00); // Green color
    kprint_str("Enjoy your stay!\n", 0x0000FF); // Blue color

    printf("Screen Resolution: %d x %d\n", boot_info->ScreenWidth, boot_info->ScreenHeight);

    while(1) __asm__("hlt");
}