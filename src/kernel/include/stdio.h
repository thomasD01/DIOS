#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void clrscr();
void printf(const char* fmt, ...);

void serial_print(const char* str);
void serial_println(const char* str);
void serial_print_dec(uint64_t value);
void serial_print_hex(uint64_t value);

#ifdef __cplusplus
}
#endif
