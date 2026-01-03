#pragma once

#include <stdint.h>
#include "bootinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint64_t* gFramebuffer;
extern uint64_t gFramebufferSize;
extern uint8_t gFramebufferPixelFormat;
extern uint32_t gFramebufferWidth;
extern uint32_t gFramebufferHeight;

void fb_init();
void put_pixel(uint32_t x, uint32_t y, uint32_t color);

#ifdef __cplusplus
}
#endif