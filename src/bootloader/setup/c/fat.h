#pragma once 

#include "stdint.h"
#include "disk.h"

#define SECTOR_SIZE           512
#define MAX_PATH_SIZE         256
#define MAX_FILE_HANDLES      10
#define ROOT_DIRECTORY_HANDLE -1

typedef struct 
{
  uint8_t BootJumpInstruction[3];
  uint8_t OEMName[8];
  uint16_t BytesPerSector;
  uint8_t SectorsPerCluster;
  uint16_t ReservedSectors;
  uint8_t NumberOfFATs;
  uint16_t RootEntries;
  uint16_t TotalSectors;
  uint8_t MediaDescriptor;
  uint16_t SectorsPerFAT;
  uint16_t SectorsPerTrack;
  uint16_t Heads;
  uint32_t HiddenSectors;
  uint32_t TotalSectorsBig;

  // Extended Boot Sector
  uint8_t DriveNumber;
  uint8_t Reserved;
  uint8_t ExtendedBootSignature;
  uint32_t SerialNumber;
  uint8_t VolumeLabel[11];
  uint8_t SystemID[8];

} __attribute__((packed)) fat_boot_sector_t;

typedef struct 
{
  uint8_t Filename[11];
  uint8_t Attributes;
  uint8_t Reserved;
  uint8_t CreationTimeTenths;
  uint16_t CreationTime;
  uint16_t CreationDate;
  uint16_t LastAccessDate;
  uint16_t HighClusterNumber;
  uint16_t LastModificationTime;
  uint16_t LastModificationDate;
  uint16_t LowClusterNumber;
  uint32_t FileSize;
} __attribute__((packed)) fat_directory_entry_t;

typedef struct 
{
  int handle;
  bool isDirectory;
  uint32_t position;
  uint32_t size;
} fat_file_t;

typedef struct {
  fat_file_t file;
  bool opened;
  uint32_t firstCluster;
  uint32_t currentCluster;
  uint32_t currentSector;
  uint8_t buffer[SECTOR_SIZE];
} fat_file_data_t;

typedef struct 
{
  union 
  {
    fat_boot_sector_t bootSector;
    uint8_t bytes[SECTOR_SIZE];
  } BS;

  fat_file_data_t rootDirectory;

  fat_file_data_t fileHandles[MAX_FILE_HANDLES];

} fat_data_t;

enum fat_attribute {
  FAT_ATTRIBUTE_READ_ONLY   = 0x01,
  FAT_ATTRIBUTE_HIDDEN      = 0x02,
  FAT_ATTRIBUTE_SYSTEM      = 0x04,
  FAT_ATTRIBUTE_VOLUME_ID   = 0x08,
  FAT_ATTRIBUTE_DIRECTORY   = 0x10,
  FAT_ATTRIBUTE_ARCHIVE     = 0x20,
  FAT_ATTRIBUTE_LONG_NAME   = FAT_ATTRIBUTE_READ_ONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID,
};

bool FAT_initialize(disk_t* disk);
fat_file_t* FAT_open(disk_t* disk, const char* filename);
uint32_t FAT_read(disk_t* disk, fat_file_t* file, uint32_t size, void* buffer);
bool FAT_readEntry(disk_t* disk, fat_file_t* file, fat_directory_entry_t* entry);
void FAT_close(fat_file_t* file);
