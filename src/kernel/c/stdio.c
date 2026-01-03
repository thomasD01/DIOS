#include <stdarg.h>
#include <stdbool.h>

#include "stdio.h"
#include "console/console.h"
#include "x86.h"

void clrscr() 
{
	kconsole_clear(0x000000); // Black color
}

void printf(const char* fmt, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, fmt);
	int len = 0;

	const char* p = fmt;
	while (*p)
	{
		if (*p == '%')
		{
			p++;
			if (*p == 's')
			{
					const char* str = va_arg(args, const char*);
					while (*str)
					{
						buffer[len++] = *str++;
					}
			} else if (*p == 'd')
			{
				int num = va_arg(args, int);
				char num_buffer[20];
				int num_len = 0;
				if (num == 0)
				{
					num_buffer[num_len++] = '0';
				} else
				{
					bool is_negative = false;
					if (num < 0)
					{
						is_negative = true;
						num = -num;
					}
					while (num > 0)
					{
						num_buffer[num_len++] = '0' + (num % 10);
						num /= 10;
					}
					if (is_negative)
					{
						num_buffer[num_len++] = '-';
					}
				}
				for (int i = num_len - 1; i >= 0; i--)
				{
						buffer[len++] = num_buffer[i];
				}
			} else if (*p == 'c')
			{
				char c = (char)va_arg(args, int);
				buffer[len++] = c;
			} else
			{
				buffer[len++] = '%';
				buffer[len++] = *p;
			}
		} else
		{
			buffer[len++] = *p;
		}
		p++;
	}
	va_end(args);
	buffer[len] = '\0';

	kprint_str(buffer, 0xFFFFFF);
}

void serial_print(const char* s) {
	while (*s) {
		x86_outb(0x3F8, *s++);
  }
}


void serial_println(const char* s) {
	serial_print(s);
	x86_outb(0x3F8, '\n');
}


void serial_print_dec(uint64_t n)
{
	uint64_t num = n;
	char buffer[20];
	int len = 0;
	if (num == 0)
	{
		buffer[len++] = '0';
	} else if (num < 0)
	{
		buffer[len++] = '-';
		num = -num;
	} else
	{
		while (num > 0)
		{
			buffer[len++] = '0' + (num % 10);
			num /= 10;
		}
	}

	char ret[len];

	for (int i = 0; i < len; i++)
	{
		ret[i] = buffer[len - i - 1];
	}

	serial_print(ret);
}

void serial_print_hex(uint64_t n) {
  const char* chars = "0123456789ABCDEF";
  for(int i = 60; i >= 0; i -= 4) {
    x86_outb(0x3F8, chars[(n >> i) & 0xF]);
  }
}