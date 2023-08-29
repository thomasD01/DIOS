#include <stdint.h>
#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"

uint8_t *KernelLoadBuffer = (uint8_t *)MEMORY_LOAD_KERNEL;
uint8_t *Kernel = (uint8_t *)MEMORY_KERNEL_ADDRESS;

typedef void (*KernelStart)();

void __attribute__((cdecl)) start(uint16_t bootDrive)
{

	clrscr();

	printf("Bootloader started\r\n");

	disk_t disk;
	if (!DISK_init(&disk, bootDrive))
	{
		printf("Disk init error\r\n");
		goto end;
	}

	if (!FAT_initialize(&disk))
	{
		printf("FAT init error\r\n");
		goto end;
	}

	printf("FAT initialized\r\n");

	// load kernel
	fat_file_t *fd = FAT_open(&disk, "/kernel.bin");
	uint32_t read;
	uint8_t *kernelBuffer = Kernel;
	while ((read = FAT_read(&disk, fd, MEMORY_LOAD_SIZE, KernelLoadBuffer)))
	{
		memcpy(kernelBuffer, KernelLoadBuffer, read);
		kernelBuffer += read;
	}
	FAT_close(fd);

	printf("Kernel loaded\r\n");

	// execute kernel
	KernelStart kernelStart = (KernelStart)Kernel;
	//kernelStart();
	
end:
	for (;;)
		;
}
