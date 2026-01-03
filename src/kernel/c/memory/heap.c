#include "heap.h"

struct HeapSegmentHeader* FirstFreeSegment = NULL;

void heap_init()
{
  void* pos = pmm_allocPage();

  // TODO virtual addresses

  FirstFreeSegment = (struct HeapSegmentHeader*)pos;
  FirstFreeSegment->length = 4096 - sizeof(struct HeapSegmentHeader);
  FirstFreeSegment->next = NULL;
  FirstFreeSegment->prev = NULL;
  FirstFreeSegment->free = 1;
  FirstFreeSegment->magic = 0xDEADBEEF;
}

void *kalloc(size_t size)
{
  if (size % 8 != 0) size += (8 - (size % 8));

  struct HeapSegmentHeader* current = FirstFreeSegment;

  while (current != NULL)
  {
    if (current->free && current->length >=size)
    {
      if (current->length > size + sizeof(struct HeapSegmentHeader) + 8)
      {
        struct HeapSegmentHeader* newHeader = (struct HeapSegmentHeader*)((uint8_t*)current + sizeof(struct HeapSegmentHeader) + size);

        newHeader->free = 1;
        newHeader->length = current->length - size - sizeof(struct HeapSegmentHeader);
        newHeader->next = current->next;
        newHeader->prev = current;
        newHeader->magic = HEAP_MAGIC;

        if (current->next != NULL)
        {
          current->next->prev = newHeader;
        }

        current->next = newHeader;
        current->length = size;
      }
      current->free = 0;
      return (void*)((uint8_t*)current + sizeof(struct HeapSegmentHeader));
    }
    current = current->next;
  }

  return NULL; // OOM (bad)
}

void kfree(void *ptr)
{
  struct HeapSegmentHeader* header = (struct HeapSegmentHeader*)((uint8_t*)ptr - sizeof(struct HeapSegmentHeader));

  if (header->magic != HEAP_MAGIC)
  {
    return;
  }

  header->free = 1;

    if (header->next && header->next->free) {
        header->length += header->next->length + sizeof(struct HeapSegmentHeader);
        header->next = header->next->next;
        if(header->next) header->next->prev = header;
    }

    if (header->prev && header->prev->free) {
        header->prev->length += header->length + sizeof(struct HeapSegmentHeader);
        header->prev->next = header->next;
        if(header->next) header->next->prev = header->prev;
    }
}