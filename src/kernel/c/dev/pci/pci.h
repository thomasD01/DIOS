#pragma once

#include <stdint.h>

typedef struct {
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Command;
    uint16_t Status;
    uint8_t  RevisionID;
    uint8_t  ProgIF;
    uint8_t  Subclass;
    uint8_t  ClassCode;
    uint8_t  CacheLineSize;
    uint8_t  LatencyTimer;
    uint8_t  HeaderType;
    uint8_t  BIST;
    // 128 more bites of device-specific data would follow 
} PCIHeader;

// Device Classes we care about
#define PCI_CLASS_MASS_STORAGE  0x01
#define PCI_SUBCLASS_SATA       0x06
#define PCI_SUBCLASS_IDE        0x01

#ifdef __cplusplus
extern "C" {
#endif
    void pci_init();
#ifdef __cplusplus
}
#endif