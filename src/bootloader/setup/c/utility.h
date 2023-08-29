#pragma once

#include <stdint.h>
#include <stdbool.h>

uint32_t align(uint32_t value, uint32_t alignment);

bool isLower(char c);
char toUpper(char c);

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))