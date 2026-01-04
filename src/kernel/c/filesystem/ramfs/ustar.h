#pragma once
#include <stdint.h>

struct UstarHeader {
  char name[100];       // Filename
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];        // Size in Octal (Base 8) ASCII
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];        // "ustar"
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];         // Pads struct to 512 bytes
};