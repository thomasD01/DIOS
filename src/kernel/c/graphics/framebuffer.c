#include "framebuffer.h"

uint64_t* gFramebuffer = NULL;
uint64_t gFramebufferSize = 0;
uint8_t gFramebufferPixelFormat = 0;
uint32_t gFramebufferWidth = 0;
uint32_t gFramebufferHeight = 0;

void fb_init()
{
  if (!gBootInfo) return;

  gFramebuffer = (uint64_t*)gBootInfo->FrameBufferBase;
  gFramebufferSize = gBootInfo->FrameBufferSize;
  gFramebufferPixelFormat = gBootInfo->PixelFormat;
  gFramebufferWidth = gBootInfo->PixelsPerScanLine;
  gFramebufferHeight = gBootInfo->ScreenHeight;

  // check if framebuffer size actually fits screen dimensions
  uint64_t expected_size =
    (uint64_t)gFramebufferWidth *
    (uint64_t)gFramebufferHeight *
    sizeof(uint32_t);

  if (gFramebufferSize < expected_size)
  {
    // invalid framebuffer size
    gFramebuffer = NULL;
    gFramebufferSize = 0;
    gFramebufferPixelFormat = 0;
    gFramebufferWidth = 0;
    gFramebufferHeight = 0;
  }
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
  if (!gFramebuffer) return;

  uint32_t stride = gBootInfo->PixelsPerScanLine;
  uint32_t index = y * stride + x;

  if (index * sizeof(uint32_t) >= gFramebufferSize) return;
    
  // cursed byte bullshit
  ((uint32_t*)gFramebuffer)[index] = color;
}