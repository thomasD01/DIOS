#include "disk.h"
#include "stdio.h"
#include "x86.h"

bool DISK_init(disk_t *disk, uint8_t drive)
{
  uint8_t driveType;
  uint16_t cylinders, sectors, heads;


  if(!x86_Disk_GetDriveParams(drive, &driveType, &cylinders, &sectors, &heads))
    return false;

  disk->drive = drive;
  disk->cylinders = cylinders +1;
  disk->heads = heads +1;
  disk->sectors = sectors;

  return true;	
}

void DISK_lbaToChs(disk_t *disk, uint32_t lba, uint16_t *cylinder, uint16_t *sector, uint16_t *head)
{
  *sector = lba % disk->sectors + 1;
  *cylinder = (lba / disk->sectors) / disk->heads;
  *head = (lba / disk->sectors) % disk->heads;
}

bool DISK_readSectors(disk_t *disk, uint32_t lba, uint8_t count, void* buffer)
{
  uint16_t cylinder, sector, head;

  DISK_lbaToChs(disk, lba, &cylinder, &sector, &head);

  for(int i=0; i<3; i++)
  {
    if(x86_Disk_Read(disk->drive, cylinder, sector, head, count, buffer))
      return true;
    
    x86_Disk_Reset(disk->drive);
  }

  return false;
}
