#include "utility.h"


uint32_t align(uint32_t number, uint32_t alignment)
{
  if(alignment == 0)
    return number;

  uint32_t remainder = number % alignment;
  return (remainder > 0) ? (number + alignment - remainder) : number;
}

bool isLower(char c)
{
  return (c >= 'a' && c <= 'z');
}

char toUpper(char c)
{
  return (c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c;
}
