.code64

/* ---------------------------------------------------------
   Function: void x86_outb(uint16_t port, uint8_t val)
   ABI: RDI = port, RSI = val
   --------------------------------------------------------- */
.global x86_outb
x86_outb:
    mov %di, %dx
    mov %sil, %al
    outb %al, %dx
    ret

/* ---------------------------------------------------------
   Function: uint8_t x86_inb(uint16_t port)
   ABI: RDI = port
   Return: AL (low byte of RAX)
   --------------------------------------------------------- */
.global x86_inb
x86_inb:
    mov %di, %dx
    inb %dx, %al
    ret

/* ---------------------------------------------------------
   Function: void x86_halt(void)
   --------------------------------------------------------- */
.global x86_halt
x86_halt:
    hlt
    ret

/* ---------------------------------------------------------
   Function: void x86_cli(void)
   --------------------------------------------------------- */
.global x86_cli
x86_cli:
    cli
    ret

/* ---------------------------------------------------------
   Function: void x86_sti(void)
   --------------------------------------------------------- */
.global x86_sti
x86_sti:
    sti
    ret
