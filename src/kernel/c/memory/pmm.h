#pragma once
#include <stdint.h>
#include <stddef.h>
#include "bootinfo.h"

#define PAGE_SIZE 4096
#define KERNEL_MEMORY_SAFEZONE 0x200000 // 2 MB

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t* Bitmap;
extern uint64_t BitmapSize;
extern uint64_t TotalRAM;
extern uint64_t FreeRAM;

static int bitmapGet(uint64_t index);
static void bitmapSet(uint64_t index);
static void bitmapUnset(uint64_t index);
static EFI_MEMORY_DESCRIPTOR* getMemoryaDescriptor(uint64_t index);

void pmm_init();
void* pmm_allocPage();
void pmm_freePage(void* addr);

#ifdef __cplusplus
}
#endif