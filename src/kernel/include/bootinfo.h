#pragma once
#include <efi.h>

typedef struct __attribute__((packed)) {
    // Memory Map Info
    EFI_MEMORY_DESCRIPTOR* mMap;
    UINTN mMapSize;
    UINTN mMapDescriptorSize;

    // Graphics Info (GOP)
    UINT64 FrameBufferBase;
    UINT64 FrameBufferSize;
    UINT32 ScreenWidth;
    UINT32 ScreenHeight;
    UINT32 PixelsPerScanLine;
    UINT8 PixelFormat;
    
    // // Runtime Services
    EFI_RUNTIME_SERVICES* RuntimeServices;
} BootInfo;

extern BootInfo* gBootInfo;