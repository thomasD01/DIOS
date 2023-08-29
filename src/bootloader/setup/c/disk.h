#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint8_t drive;
  uint16_t cylinders;
  uint16_t sectors;
  uint16_t heads;
} disk_t;

bool DISK_init(disk_t *disk, uint8_t drive);
bool DISK_readSectors(disk_t *disk, uint32_t lba, uint8_t count, void* buffer);
