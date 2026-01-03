#pragma once

#include <stdint.h>
#include <stddef.h>

#define CONSOLE_PADDING 2
#define CONSOLE_FONT_WIDTH 8
#define CONSOLE_FONT_HEIGHT 8

#ifdef __cplusplus
extern "C" {
#endif


extern size_t cursor_x;
extern size_t cursor_y;
extern size_t console_width;
extern size_t console_height;

void kconsole_init();
void kconsole_clear(uint32_t color);
void kprint_str(const char* str, uint32_t color);

#ifdef __cplusplus
}
#endif