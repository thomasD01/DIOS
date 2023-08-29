#pragma one

// 0x00000000 - 0x000003FF - interrupt vector table
// 0x00000400 - 0x000004FF - Bios data

#define MEMORY_MIN           0x00000500
#define MEMORY_MAX           0x00080000

// 0x00000500 - 0x00010500 - FAT driver
#define MEMORY_FAT_ADDRESS   ((void*) 0x00200000)  // segment:offset (ssssoooo)
#define MEMORY_FAT_SIZE      0x00010000

#define MEMORY_LOAD_KERNEL  ((void*) 0x00300000)
#define MEMORY_LOAD_SIZE     0x00010000

// 0x00020000 - 0x00030000 - setup.bin

// 0x00030000 - 0x00080000 - free

// 0x00080000 - 0x0009FFFF - extended BIOS data
// 0x000A0000 - 0x000C7FFF - video memory
// 0x000C8000 - 0x000FFFFF - BIOS ROM

#define MEMORY_KERNEL_ADDRESS ((void*) 0x00100000)
