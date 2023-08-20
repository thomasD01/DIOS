#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

typedef uint8_t bool;
#define true 1
#define false 0

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

} __attribute__((packed))  boot_sector_t;

typedef struct {
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
} __attribute__((packed))  directory_entry_t;

uint32_t g_root_end = 0;
uint8_t* g_fat = NULL;
directory_entry_t* g_root_directory = NULL;
boot_sector_t g_boot_sector;


bool readBootSector(FILE* disk)
{
  return fread(&g_boot_sector, sizeof(boot_sector_t), 1, disk) > 0;
}

bool readSectors(FILE* disk, uint32_t lba, uint32_t count, void* buffer)
{
  bool ok = true;
  ok = ok && (fseek(disk, lba * g_boot_sector.BytesPerSector, SEEK_SET) == 0);
  ok = ok && (fread(buffer, g_boot_sector.BytesPerSector, count, disk) == count);
  return ok;
}

bool readFAT(FILE* disk)
{
  g_fat = (uint8_t*) malloc(g_boot_sector.SectorsPerFAT * g_boot_sector.BytesPerSector);
  return readSectors(disk, g_boot_sector.ReservedSectors, g_boot_sector.SectorsPerFAT, g_fat);
}

bool readRootDirectory(FILE* disk)
{
  uint32_t lba = g_boot_sector.ReservedSectors + g_boot_sector.SectorsPerFAT * g_boot_sector.NumberOfFATs;
  uint32_t size = g_boot_sector.RootEntries * sizeof(directory_entry_t);
  uint32_t count = size / g_boot_sector.BytesPerSector;
  if(size % g_boot_sector.BytesPerSector)
    count++;

  g_root_end = lba + count;

  g_root_directory = (directory_entry_t*) malloc(count * g_boot_sector.BytesPerSector);
  return readSectors(disk, lba, count, g_root_directory);
}

directory_entry_t* findFile(const char* name)
{
  for(int i = 0; i < g_boot_sector.RootEntries; i++)
  {
    if(g_root_directory[i].Filename[0] == 0)
      return NULL;

    if(g_root_directory[i].Filename[0] == 0xE5)
      continue;

    if(g_root_directory[i].Attributes & 0x0F)
      continue;

    if(strncmp((const char*) g_root_directory[i].Filename, name, 11) == 0)
      return &g_root_directory[i];
  }

  return NULL;
}

bool readFile(directory_entry_t *fileEntry, FILE* disk, uint8_t* buffer)
{
  bool ok = true;
  uint16_t cluster = fileEntry->LowClusterNumber;

  do 
  {
    uint32_t lba = g_root_end + (cluster - 2) * g_boot_sector.SectorsPerCluster;
    ok = ok && readSectors(disk, lba, g_boot_sector.SectorsPerCluster, buffer);
    cluster = *((uint16_t*) (g_fat + cluster * 2));

    buffer += g_boot_sector.SectorsPerCluster * g_boot_sector.BytesPerSector;

    uint32_t fatIndex = cluster * 3 / 2;

    if(cluster % 2 == 0)
      cluster = (*(uint16_t*)(g_fat + fatIndex)) >> 4;
  }
  while(ok && cluster < 0x0FF8);

  return ok;
}

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    printf("Usage: %s <image> <filename>\n", argv[0]);
    return -1;
  }

  FILE *disk = fopen(argv[1], "rb");
  if(!disk)
  {
    printf("Failed to open %s\n", argv[1]);
    return -1;
  }

  if(!readBootSector(disk))
  {
    printf("Failed to read boot sector\n");
    return -2;
  }

  if(!readFAT(disk))
  {
    printf("Failed to read FAT\n");
    free(g_fat);
    return -3;
  }

  if(!readRootDirectory(disk))
  {
    printf("Failed to read root directory\n");
    free(g_fat);
    free(g_root_directory);
    return -4;
  }

  directory_entry_t* entry = findFile(argv[2]);
  if(!entry)
  {
    printf("File %s not found\n", argv[2]);
    free(g_fat);
    free(g_root_directory);
    return -5;
  }

  uint8_t* buffer = (uint8_t*) malloc(entry->FileSize + g_boot_sector.BytesPerSector);
  if(!readFile(entry, disk, buffer))
  {
    printf("Failed to read file\n");
    free(g_fat);
    free(g_root_directory);
    free(buffer);
    return -6;
  }

  for(size_t i=0; i<entry->FileSize; i++)
  {
    if(isprint(buffer[i]))
      printf("%c", buffer[i]);
    else
      printf("<%02x>", buffer[i]);
  }

  printf("\n");

  free(g_fat);
  free(g_root_directory);
  free(buffer);
  fclose(disk);
  return 0;
}
