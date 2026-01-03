#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern void x86_outb(uint16_t port, uint8_t val);
extern uint8_t x86_inb(uint16_t port);
extern void x86_halt(void);
extern void x86_cli(void);
extern void x86_sti(void);

#ifdef __cplusplus
}
#endif