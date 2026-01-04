#include "vfs.h"

#include "stdio.h"
#include "ustar.h"

uint64_t octal_to_int(const char* str, size_t size)
{
  uint64_t res = 0;
  for (size_t i = 0; i < size; i++)
  {
    if (str[i] < '0' || str[i] > '7')
    {
      break;
    }
    res = (res << 3) + (str[i] - '0');
  }

  return res;
}

void vfs_list_files(BootInfo* boot_info)
{
  if (boot_info->RamdiskSize == 0 || boot_info->RamdiskBase == NULL) {
    printf("No ramdisk loaded.\n");
    return;
  }

  uint8_t* cursor = (uint8_t*)boot_info->RamdiskBase;
    
  while ((uint64_t)(cursor - (uint8_t*)boot_info->RamdiskBase) < boot_info->RamdiskSize) {
    struct UstarHeader* header = (struct UstarHeader*)cursor;

    if (header->name[0] == 0)
    {
      break; // End of archive
    }

    uint64_t size = octal_to_int(header->size, 11);

    printf("Found file: %s, Size: %d bytes\n", header->name, size);
    
    uint64_t jump = 512 + size;
    if (size % 512 != 0)
    {
        jump += 512 - (size % 512);
    }
    
    cursor += jump;
  }
}