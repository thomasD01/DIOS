#include "graphics.h"
#include "font.h"

#define PADDING 2

uint32_t CursorX = PADDING;
uint32_t CursorY = PADDING;

void put_pixel(uint32_t x, uint32_t y, uint32_t color) 
{
  if (!gBootInfo) return;

  uint32_t* fb = (uint32_t*)gBootInfo->FrameBufferBase;
  uint32_t stride = gBootInfo->PixelsPerScanLine;
  uint32_t index = y * stride + x;
    
  fb[index] = color;
}
void kprint_char(char c, uint32_t color) 
{
  if (c == '\n') 
  {
    CursorX = PADDING;
    CursorY += 8;
    return;
  }

  uint8_t* glyph = font8x8_basic[(int)c];
  for (int y = 0; y < 8; y++)
  {
    for (int x = 0; x < 8; x++)
    {
      if (glyph[y] & (1 << x))
      {
        put_pixel(CursorX + x, CursorY + y, color);
      }
    }
  }

  CursorX += 8;

  if (CursorX >= gBootInfo->ScreenWidth)
  {
    CursorX = PADDING;
    CursorY += 8;
  }
}
void kprint_str(const char* str, uint32_t color)
{
    while (*str) 
    {
      kprint_char(*str, color);
      str++;
    }
}
void kclear_screen(uint32_t color)
{
  for (uint32_t i = 0; i < gBootInfo->FrameBufferSize; i++) {
    ((uint32_t*)gBootInfo->FrameBufferBase)[i] = color;
  }
  CursorX = PADDING;
  CursorY = PADDING;
}
void kset_cursor(uint32_t x, uint32_t y)
{
  CursorX = x;
  CursorY = y;
}