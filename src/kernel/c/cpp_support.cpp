#include <stddef.h>
#include <stdint.h>

#include "memory/heap.h"

// Called when a pure virtual function is invoked
extern "C" void __cxa_pure_virtual() {
  // TODO handle this properly
  while (1) __asm__ ("hlt");
}

void *__dso_handle;
extern "C" int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso) {
  // TODO implement proper destructor handling
  (void)destructor; (void)arg; (void)dso;
  return 0;
}

void* operator new(size_t size) {
    return kalloc(size);
}

void* operator new[](size_t size) {
    return kalloc(size);
}

void operator delete(void* p) {
    kfree(p);
}

void operator delete(void* p, size_t size) {
    (void)size; // Unused
    kfree(p);
}

void operator delete[](void* p) {
    kfree(p);
}

void operator delete[](void* p, size_t size) {
    (void)size;
    kfree(p);
}