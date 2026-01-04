#pragma once

#include <stdint.h>
#include <stddef.h>

#include "bootinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

void vfs_list_files(BootInfo* boot_info);

#ifdef __cplusplus
}
#endif