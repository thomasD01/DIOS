#pragma once
#include <stdint.h>
#include "bootinfo.h"

void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void kprint_char(char c, uint32_t color);
void kprint_str(const char* str, uint32_t color);
void kclear_screen(uint32_t color);
void kset_cursor(uint32_t x, uint32_t y);
