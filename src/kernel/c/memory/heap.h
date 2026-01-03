#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "pmm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HEAP_MAGIC 0xDEADBEEF
#define HEAP_START_ADDR 0x10000000

struct HeapSegmentHeader 
{
  size_t length;
  struct HeapSegmentHeader* next;
  struct HeapSegmentHeader* prev;
  bool free;
  uint32_t magic;
};

extern struct HeapSegmentHeader* FirstFreeSegment;

void heap_init();
void* kalloc(size_t size);
void kfree(void* ptr);

#ifdef __cplusplus
}
#endif