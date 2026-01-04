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
   Function: void x86_outb(uint16_t port, uint16_t val)
   ABI: RDI = port, RSI = val
   --------------------------------------------------------- */
.global x86_outw
x86_outw:
    mov %di, %dx
    mov %si, %ax
    outw %ax, %dx
    ret

/* ---------------------------------------------------------
   Function: void x86_outb(uint16_t port, uint32_t val)
   ABI: RDI = port, RSI = val
   --------------------------------------------------------- */
.global x86_outl
x86_outl:
    mov %di, %dx
    mov %esi, %eax
    outl %eax, %dx
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
   Function: uint16_t x86_inw(uint16_t port)
   ABI: RDI = port
   Return: AX (low word of RAX)
   --------------------------------------------------------- */
.global x86_inw
x86_inw:
    mov %di, %dx
    inw %dx, %ax
    ret

/* ---------------------------------------------------------
   Function: uint32_t x86_inl(uint16_t port)
   ABI: RDI = port
   Return: EAX (low double word of RAX)
   --------------------------------------------------------- */
.global x86_inl
x86_inl:
    mov %di, %dx
    inl %dx, %eax
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
