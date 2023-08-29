#include "fat.h"

#include "stdio.h"
#include "string.h"
#include "disk.h"
#include "memdefs.h"
#include "memory.h"
#include "utility.h"

#include <stddef.h>

static fat_data_t* g_fatData;
static uint8_t* g_fat = NULL;
static uint32_t g_dataSectionLBA;

bool FAT_readBootSector(disk_t *disk)
{
  return DISK_readSectors(disk, 0, 1, g_fatData->BS.bytes);
}

bool FAT_readFAT(disk_t *disk)
{
  return DISK_readSectors(disk, g_fatData->BS.bootSector.ReservedSectors, g_fatData->BS.bootSector.SectorsPerFAT, g_fat);
}

uint32_t FAT_clusterToLba(uint32_t cluster)
{
  return g_dataSectionLBA + (cluster - 2) * g_fatData->BS.bootSector.SectorsPerCluster;
}

bool FAT_findFile(disk_t *disk, fat_file_t* file, const char *name, fat_directory_entry_t *entry)
{
  char fatname[12];
  fat_directory_entry_t current_entry;

  // convert name to FAT format
  memset(fatname, ' ', sizeof(fatname));
  fatname[11] = '\0';

  const char *ext = strchr(name, '.');

  if (ext == NULL)
  {
    ext = name + 11;
  }

  for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
  {
    fatname[i] = toUpper(name[i]);
  }

  if (ext != NULL)
  {
    for (int i = 0; i < 3 && ext[i + 1]; i++)
    {
      fatname[8 + i] = toUpper(ext[i + 1]);
    }
  }

  while (FAT_readEntry(disk, file, &current_entry))
  {
    if (memcmp(fatname, current_entry.Filename, 11) == 0)
    {
      *entry = current_entry;
      return true;
    }
  }

  return false;
}

uint32_t FAT_nextCluster(uint32_t cluster)
{
  uint32_t fatIndex = cluster * 3 / 2;

  if (cluster % 2 == 0)
  {
    return (*(uint16_t*)(g_fat + fatIndex)) & 0x0FFF;
  }
  else
  {
    return (*(uint16_t*)(g_fat + fatIndex)) >> 4;
  }
}

//-------------------------------------------------------------------------------------------------
// Public API
//-------------------------------------------------------------------------------------------------
bool FAT_initialize(disk_t *disk)
{

  g_fatData = (fat_data_t*)MEMORY_FAT_ADDRESS;

  //---------------------------------------------
  // read boot sector
  //---------------------------------------------
  if (!FAT_readBootSector(disk))
  {
    printf("Failed to read boot sector\r\n");
    return false;
  }

  //---------------------------------------------
  // read file allocation table
  //---------------------------------------------
  g_fat = (uint8_t*)g_fatData + sizeof(fat_data_t);
  uint32_t fatSize = g_fatData->BS.bootSector.BytesPerSector * g_fatData->BS.bootSector.SectorsPerFAT;

  if (sizeof(fat_data_t) + fatSize > MEMORY_FAT_SIZE)
  {
    printf("FAT is too large to fit in memory! Required %u, got %u\r\n", sizeof(fat_data_t) + fatSize, MEMORY_FAT_SIZE);
    return false;
  }

  if (!FAT_readFAT(disk))
  {
    printf("Failed to read FAT\r\n");
    return false;
  }

  //---------------------------------------------
  // open root directory
  //---------------------------------------------
  uint32_t rootDirLBA = g_fatData->BS.bootSector.ReservedSectors + g_fatData->BS.bootSector.SectorsPerFAT * g_fatData->BS.bootSector.NumberOfFATs;
  uint32_t rootDirSize = sizeof(fat_directory_entry_t) * g_fatData->BS.bootSector.RootEntries;

  g_fatData->rootDirectory.file.handle = ROOT_DIRECTORY_HANDLE;
  g_fatData->rootDirectory.file.isDirectory = true;
  g_fatData->rootDirectory.file.position = 0;
  g_fatData->rootDirectory.file.size = sizeof(fat_directory_entry_t) * g_fatData->BS.bootSector.RootEntries;
  ;
  g_fatData->rootDirectory.opened = true;
  g_fatData->rootDirectory.firstCluster = rootDirLBA;
  g_fatData->rootDirectory.currentCluster = rootDirLBA;
  g_fatData->rootDirectory.currentSector = 0;

  if (!DISK_readSectors(disk, rootDirLBA, 1, g_fatData->rootDirectory.buffer))
  {
    printf("Failed to read root directory\r\n");
    return false;
  }

  //---------------------------------------------
  // calculate data section
  //---------------------------------------------
  uint32_t rootDirSectors = (rootDirSize + g_fatData->BS.bootSector.BytesPerSector - 1) / g_fatData->BS.bootSector.BytesPerSector;
  g_dataSectionLBA = rootDirLBA + rootDirSectors;

  //---------------------------------------------
  // reset opened files
  //---------------------------------------------
  for (int i = 0; i < MAX_FILE_HANDLES; i++)
  {
    g_fatData->fileHandles[i].opened = false;
  }

  return true;
}
fat_file_t* FAT_openEntry(disk_t *disk, fat_directory_entry_t *entry)
{

  // find empty handle
  int handle = -1;
  for (int i = 0; i < MAX_FILE_HANDLES; i++)
  {
    if (!g_fatData->fileHandles[i].opened)
    {
      handle = i;
      break;
    }
  }

  if (handle == -1)
  {
    printf("No free file handles\r\n");
    return NULL;
  }

  // setup
  fat_file_data_t* fileHandle = &g_fatData->fileHandles[handle];
  fileHandle->file.handle = handle;
  fileHandle->file.isDirectory = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
  fileHandle->file.position = 0;
  fileHandle->file.size = entry->FileSize;
  fileHandle->firstCluster = entry->LowClusterNumber + ((uint32_t)entry->HighClusterNumber << 16);
  fileHandle->currentCluster = fileHandle->firstCluster;
  fileHandle->currentSector = 0;

  // open file
  if (!DISK_readSectors(disk, FAT_clusterToLba(fileHandle->currentCluster), 1, fileHandle->buffer))
  {
    printf("Failed to read file\r\n");
    return NULL;
  }

  fileHandle->opened = true;

  return &fileHandle->file;
}
fat_file_t* FAT_open(disk_t *disk, const char *filename)
{
  char buffer[MAX_PATH_SIZE];

  // skip leading slash
  if (filename[0] == '/')
    filename++;

  fat_file_t* current = &g_fatData->rootDirectory.file;

  while (*filename)
  {
    bool isLast = false;
    const char *delim = strchr(filename, '/');
    if (delim != NULL)
    {
      memcpy(buffer, filename, delim - filename);
      buffer[delim - filename + 1] = '\0';
      filename = delim + 1;
    }
    else
    {
      unsigned len = strlen(filename);
      memcpy(buffer, filename, len);
      buffer[len + 1] = '\0';
      filename += len;
      isLast = true;
    }

    // find directory entry
    fat_directory_entry_t entry;
    if (FAT_findFile(disk, current, buffer, &entry))
    {
      // close current entry
      FAT_close(current);

      // check for directory
      if (!isLast && (entry.Attributes & FAT_ATTRIBUTE_DIRECTORY == 0))
      {
        printf("%s not a Directory\r\n", buffer);
        return NULL;
      }

      // open new entry
      current = FAT_openEntry(disk, &entry);
    }
    else
    {
      FAT_close(current);
      printf("Failed to find %s\r\n", buffer);
      return NULL;
    }
  }

  return current;
}
uint32_t FAT_read(disk_t *disk, fat_file_t* file, uint32_t size, void *buffer)
{
  fat_file_data_t* fd = (file->handle == ROOT_DIRECTORY_HANDLE)
                             ? &g_fatData->rootDirectory
                             : &g_fatData->fileHandles[file->handle];

  uint8_t *u8Buffer = (uint8_t *)buffer;

  // don't read past the end of the file
  if (!fd->file.isDirectory)
    size = min(size, fd->file.size - fd->file.position);

  while (size > 0)
  {
    uint32_t leftInBuffer = SECTOR_SIZE - (fd->file.position % SECTOR_SIZE);
    uint32_t take = min(size, leftInBuffer);

    memcpy(u8Buffer, fd->buffer + fd->file.position % SECTOR_SIZE, take);
    u8Buffer += take;
    fd->file.position += take;
    size -= take;

    // See if we need to read more data
    if (leftInBuffer == take)
    {
      // Special handling for root directory
      if (fd->file.handle == ROOT_DIRECTORY_HANDLE)
      {
        ++fd->currentCluster;

        // read next sector
        if (!DISK_readSectors(disk, fd->currentCluster, 1, fd->buffer))
        {
          printf("FAT: (ROOT) read error! cluster: %u \r\n", fd->currentCluster);
          break;
        }
      }
      else
      {
        // calculate next cluster & sector to read
        if (++fd->currentSector >= g_fatData->BS.bootSector.SectorsPerCluster)
        {
          fd->currentSector = 0;
          fd->currentCluster = FAT_nextCluster(fd->currentCluster);
        }

        if (fd->currentCluster >= 0xFF8)
        {
          // Mark end of file
          fd->file.size = fd->file.position;
          break;
        }

        // read next sector
        if (!DISK_readSectors(disk, FAT_clusterToLba(fd->currentCluster) + fd->currentSector, 1, fd->buffer))
        {
          printf("FAT: read error!\r\n");
          break;
        }
      }
    }
  }

  return u8Buffer - (uint8_t *)buffer;
}
bool FAT_readEntry(disk_t *disk, fat_file_t* file, fat_directory_entry_t *entry)
{
  return FAT_read(disk, file, sizeof(fat_directory_entry_t), entry) == sizeof(fat_directory_entry_t);
}
void FAT_close(fat_file_t* file)
{
  if (file->handle == ROOT_DIRECTORY_HANDLE)
  {
    file->position = 0;
    g_fatData->rootDirectory.currentCluster = g_fatData->rootDirectory.firstCluster;
  }
  else
  {
    g_fatData->fileHandles[file->handle].opened = false;
  }
}
 