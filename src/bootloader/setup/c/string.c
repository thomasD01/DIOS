#include "string.h"
#include <stdint.h>
#include <stddef.h>

const char* strchr(const char* str, char c)
{
  if(str == NULL)
    return NULL;

  while(*str)
  {
    if(*str == c)
      return str;

    str++;
  }

  return NULL;
}

char* strcpy(char* dest, const char* src)
{
  char *ret = dest;

  if(dest == NULL)
    return NULL;

  if(src == NULL)
  {
    *dest = '\0';
    return dest;
  }

  while(*src)
  {
    *dest = *src;
    dest++;
    src++;
  }

  *dest = '\0';

  return ret;
}

unsigned strlen(const char* str)
{
  int len = 0;

  while(*str)
  {
    ++len;
    ++str;
  }

  return len;
}
