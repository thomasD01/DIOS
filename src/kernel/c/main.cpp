#include <stdint.h>
#include "bootinfo.h"
#include "stdio.h"
#include "x86.h"
#include "idt.h"
#include "memory/pmm.h"
#include "memory/heap.h"
#include "console/console.h"
#include "console/shell.hpp"
#include "graphics/framebuffer.h"

BootInfo* gBootInfo = NULL;
Shell* global_shell;

extern "C" void kernel_main(BootInfo* boot_info) {
  idt_init();

  gBootInfo = boot_info;

  fb_init();
  kconsole_init();

  pmm_init();
  heap_init();

  serial_println("Kernel Initialized!");

  clrscr();

  kprint_str("Welcome to DIOS Kernel!\n", 0xFFFFFF); // White color
  kprint_str("Graphics Initialized.\n", 0x00FF00); // Green color
  kprint_str("Enjoy your stay!\n", 0x0000FF); // Blue color

  printf("Screen Resolution: %d x %d\n", boot_info->ScreenWidth, boot_info->ScreenHeight);
  printf("Pixels Per Scan Line: %d\n", boot_info->PixelsPerScanLine);
  printf("Framebuffer Address: %d\n", boot_info->FrameBufferBase);
  printf("Framebuffer Size: %d bytes\n", boot_info->FrameBufferSize);
  printf("console width: %d characters\n", console_width);
  printf("console height: %d characters\n", console_height);

  global_shell = new Shell();
  global_shell->init();

  for (size_t i=0; i<100; i++) {
    printf("Filling test %d/100\n", i);
  }

  while(1) {
    global_shell->update();
    __asm__("hlt");
  }
}