#pragma once
#include <efi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed)) {
    // Memory Map Info
    EFI_MEMORY_DESCRIPTOR* mMap;
    uint64_t mMapSize;
    uint64_t mMapDescriptorSize;

    // Graphics Info (GOP)
    uint64_t FrameBufferBase;
    uint64_t FrameBufferSize;
    uint32_t ScreenWidth;
    uint32_t ScreenHeight;
    uint32_t PixelsPerScanLine;
    uint8_t PixelFormat;

    // ramdisk info
    void* RamdiskBase;
    uint64_t RamdiskSize;
    
    // // Runtime Services
    EFI_RUNTIME_SERVICES* RuntimeServices;
} BootInfo;

extern BootInfo* gBootInfo;

#ifdef __cplusplus
}
#endif
