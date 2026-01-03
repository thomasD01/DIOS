#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* dst, const void* src, uint64_t num);
void* memset(void* ptr, int value, uint64_t num);
int memcmp(const void* ptr1, const void* ptr2, uint64_t num);

#ifdef __cplusplus
}
#endif