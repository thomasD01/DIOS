#include "memory.h"

void* memcpy(void* dst, const void* src, uint16_t size)
{
  uint8_t* dstPtr = (uint8_t*)dst;
  const uint8_t* srcPtr = (const uint8_t*)src;

  for(uint16_t i = 0; i < size; i++)
  {
    dstPtr[i] = srcPtr[i];
  }

  return dst;
}

void* memset(void* dst, int value, uint16_t size)
{
  uint8_t* dstPtr = (uint8_t*)dst;

  for(uint16_t i = 0; i < size; i++)
  {
    dstPtr[i] = value;
  }

  return dst;
}

int memcmp(const void* ptr1, const void* ptr2, uint16_t num)
{
  const uint8_t* p1 = (const uint8_t*)ptr1;
  const uint8_t* p2 = (const uint8_t*)ptr2;

  for(uint16_t i = 0; i < num; i++)
    if(p1[i] != p2[i])
      return 1;

  return 0;
}
