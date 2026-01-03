#include "console.h"
#include "memory.h"
#include "font.h"
#include "../graphics/framebuffer.h"
#include "stdio.h"

size_t cursor_x = 0;
size_t cursor_y = 0;
size_t console_width = 0;
size_t console_height = 0;

uint32_t current_background_color = 0x000000; // Default black

void kconsole_init() 
{
  if (!gFramebuffer) 
  {
    return;
  }

  console_width = (gFramebufferWidth - 2*CONSOLE_PADDING) / CONSOLE_FONT_WIDTH;
  console_height = (gFramebufferHeight - CONSOLE_PADDING) / (CONSOLE_FONT_HEIGHT + CONSOLE_PADDING);
}
void kconsole_clear(uint32_t color)
{
  if (!gFramebuffer) 
  {
    return;
  }

  for (size_t i = 0; i < gFramebufferSize; i++) 
  {
    ((uint32_t*)gFramebuffer)[i] = color;
  }

  cursor_x = 0;
  cursor_y = 0;
  current_background_color = color;
}

void kscroll(uint32_t pixels)
{
  if (!gFramebuffer)
  {
    return;
  }

    uint32_t line_size = gFramebufferWidth * 4;
    uint32_t bytes_to_move = (gFramebufferHeight - pixels) * line_size;
    uint32_t bytes_to_clear = pixels * line_size;
    
    memcpy(gFramebuffer, (uint8_t*)gFramebuffer + bytes_to_clear, bytes_to_move);
    memset((uint8_t*)gFramebuffer + bytes_to_move, 0, bytes_to_clear);
}

void kconsole_put_char(char c, uint32_t color)
{
  if (!gFramebuffer) 
  {
    return;
  }

  if (c == '\n') 
  {
    cursor_x = 0;
    cursor_y++;
  } else 
  {
    size_t x_offset = cursor_x * CONSOLE_FONT_WIDTH;
    size_t y_offset = cursor_y * (CONSOLE_FONT_HEIGHT + CONSOLE_PADDING) + CONSOLE_PADDING;

    if (x_offset == 0)
    {
      x_offset = CONSOLE_PADDING;
    }

    for(size_t i=0; i<CONSOLE_FONT_HEIGHT; i++) 
    {
      for(size_t j=0; j<CONSOLE_FONT_WIDTH; j++) 
      {
        if (font8x8_basic[(uint8_t)c][i] & (1 << j)) 
        {
          put_pixel(x_offset + j, y_offset + i, color);
        } else 
        {
          put_pixel(x_offset + j, y_offset + i, current_background_color);
        }
      }
    }
  
    cursor_x++;
  
    if (cursor_x >= console_width) 
    {
      cursor_x = 0;
      cursor_y++;
    }
  }

  if (cursor_y >= console_height)
  {
    kscroll(CONSOLE_FONT_HEIGHT + CONSOLE_PADDING);
    cursor_y = console_height - 1;
  }
}

void kprint_str(const char* str, uint32_t color)
{
  if (!gFramebuffer) 
  {
    return;
  }

  while (*str) 
  {
    kconsole_put_char(*str++, color);
  }
}
