#include <stdint.h>
#include <stddef.h>

#include "stdio.h"
#include "pci.h"
#include "x86.h"

static inline uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  uint32_t address;
  uint32_t lbus  = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
  x86_outl(0xCF8, address);
  return x86_inl(0xCFC);
}

void pci_get_header(uint8_t bus, uint8_t slot, uint8_t func, PCIHeader* header)
{
  uint32_t* struct_ptr = (uint32_t*)header;
  
  for (int i = 0; i < 4; i++) 
  {
    struct_ptr[i] = pci_config_read(bus, slot, func, i * 4);
  }
}

void check_device(uint8_t bus, uint8_t device) {
  uint8_t function = 0;

  PCIHeader header;
  pci_get_header(bus, device, function, &header);

  if (header.VendorID == 0xFFFF) return;

  printf("PCI [%d:%d] Vendor=%x Device=%x ", bus, device, header.VendorID, header.DeviceID);

  if (header.ClassCode == PCI_CLASS_MASS_STORAGE) 
  {
    if (header.Subclass == PCI_SUBCLASS_SATA) 
    {
      printf("[AHCI Found!]\n");
      
      uint32_t bar5 = pci_config_read(bus, device, function, 0x24);
      printf("-> AHCI BAR5: 0x%x\n", bar5);
        
    } else if (header.Subclass == PCI_SUBCLASS_IDE) 
    {
      printf("[IDE]\n");
    }
  } else 
  {
    printf("\n");
  }
}

void pci_init() {
  printf("Initializing PCI...\n");

  for (uint8_t bus = 0; bus < 256; bus++) {
    for (uint8_t device = 0; device < 32; device++) {
      check_device(bus, device);
    }
  }

  printf("PCI Initialization Complete.\n");
}