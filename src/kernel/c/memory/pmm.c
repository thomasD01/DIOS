#include "pmm.h"

uint8_t* bitmap = NULL;
uint64_t bitmapSize = 0;
uint64_t TotalRAM = 0;
uint64_t FreeRAM = 0;

static int bitmapGet(uint64_t index) 
{
  uint64_t byteIndex = index / 8;
  uint8_t bitIndex = index % 8;
  return (bitmap[byteIndex] >> bitIndex) & 1;
}

static void bitmapSet(uint64_t index) 
{
  uint64_t byteIndex = index / 8;
  uint8_t bitIndex = index % 8;
  bitmap[byteIndex] |= (1 << bitIndex);
}

static void bitmapUnset(uint64_t index) 
{
  uint64_t byteIndex = index / 8;
  uint8_t bitIndex = index % 8;
  bitmap[byteIndex] &= ~(1 << bitIndex);
}

static EFI_MEMORY_DESCRIPTOR* getMemoryDescriptor(uint64_t index)
{
  return (EFI_MEMORY_DESCRIPTOR*)((uint64_t)gBootInfo->mMap + (index * gBootInfo->mMapDescriptorSize));
}

void pmm_init()
{
  if (gBootInfo == NULL)
  {
    return;
  }

  uint64_t mMapEntries = gBootInfo->mMapSize / gBootInfo->mMapDescriptorSize;
  uint64_t highestAddr = 0;

  for (uint64_t i = 0; i < mMapEntries; i++)
  {
    EFI_MEMORY_DESCRIPTOR* desc = getMemoryDescriptor(i);
    
    uint64_t endAddr = desc->PhysicalStart + (desc->NumberOfPages * PAGE_SIZE);
    if (endAddr > highestAddr) highestAddr = endAddr;
    
    if (desc->Type == 7) FreeRAM += (desc->NumberOfPages * PAGE_SIZE);
  }

  TotalRAM = highestAddr;

  uint64_t totalPages = TotalRAM / PAGE_SIZE;
  bitmapSize = totalPages / 8 + 1;

  for (uint64_t i = 0; i < mMapEntries; i++) 
  {
    EFI_MEMORY_DESCRIPTOR* desc = getMemoryDescriptor(i);
    if (desc->Type == 7) 
    {
      if (desc->NumberOfPages * PAGE_SIZE >= bitmapSize) 
      {
        bitmap = (uint8_t*)desc->PhysicalStart;
        break;
      }
    }
  }

  for(uint64_t i = 0; i < bitmapSize; i++) bitmap[i] = 0xFF;

  for (uint64_t i = 0; i < mMapEntries; i++) 
  {
    EFI_MEMORY_DESCRIPTOR* desc = getMemoryDescriptor(i);
    
    if (desc->Type == 7)
    {
      uint64_t startPage = desc->PhysicalStart / PAGE_SIZE;
      for (uint64_t j = 0; j < desc->NumberOfPages; j++)
      {
        bitmapUnset(startPage + j);
      }
    }
  }

  uint64_t bitmapPageStart = (uint64_t)bitmap / PAGE_SIZE;
  uint64_t bitmapPageCount = bitmapSize / PAGE_SIZE + 1;
  for (uint64_t i = 0; i < bitmapPageCount; i++) 
  {
    bitmapSet(bitmapPageStart + i);
  }

  for (uint64_t i = 0; i < (KERNEL_MEMORY_SAFEZONE / PAGE_SIZE); i++) 
  {
    bitmapSet(i);
  }
}

void* pmm_allocPage() 
{
  for (uint64_t i = 0; i < (TotalRAM / PAGE_SIZE); i++) 
  {
    if (!bitmapGet(i)) 
    {
      bitmapSet(i);
      FreeRAM -= PAGE_SIZE;
      return (void*)(i * PAGE_SIZE);
    }
  }
  return NULL; // OOM (bad)
}

void pmm_freePage(void* addr) 
{
  uint64_t pageIndex = (uint64_t)addr / PAGE_SIZE;
  bitmapUnset(pageIndex);
  FreeRAM += PAGE_SIZE;
}