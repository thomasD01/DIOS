bits 16

section .entry

extern __bss_start
extern __end

extern start
global entry

entry:
    cli
    
    ; save boot drive
    mov [g_bootDrive], dl

    ; setup stack
    mov ax, ds
    mov ss, ax
    mov sp, 0xFFF0
    mov bp, sp 

    ; switch to 32-bit protected mode
    call EnableA20
    call LoadGDT

    ; set protected mode bit in cr0
    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    ; far jump to 32-bit code
    jmp dword 08h:.pmode

; 32-bit code
.pmode:
    [bits 32]

    ; segment registers
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

    ; clear bss
    mov edi, __bss_start
    mov ecx, __end
    sub ecx, edi
    mov al, 0
    cld
    rep stosb

    ; boot drive in dl, call start
    xor edx, edx
    mov dl, [g_bootDrive]
    push edx
    call start

    ; hang
    cli
    hlt

; enable a20
EnableA20:
    [bits 16]

    ; disable keyboard
    call A20WaitInput
    mov al, KbdControllerDisableKeyboard
    out KbdControllerCommandPort, al

    ; read control output port
    call A20WaitInput
    mov al, KbdControllerReadControlOutputPort
    out KbdControllerCommandPort, al

    ; wait for output
    call A20WaitOutput
    in al, KbdControllerDataPort
    push eax

    ; write control output port
    call A20WaitInput
    mov al, KbdControllerWriteControlOutputPort
    out KbdControllerCommandPort, al

    ; write output
    call A20WaitInput
    pop eax
    or al, 2
    out KbdControllerDataPort, al

    ; enable keyboard
    call A20WaitInput
    mov al, KbdControllerEnableKeyboard
    out KbdControllerCommandPort, al

    ; wait for input buffer empty
    call A20WaitInput
    ret

A20WaitInput:
    [bits 16]
    ; wait until input buffer can be read
    in al, KbdControllerCommandPort
    test al, 2
    jnz A20WaitInput
    ret

A20WaitOutput:
    [bits 16]
    ; wait until output buffer can be written
    in al, KbdControllerCommandPort
    test al, 1
    jz A20WaitOutput
    ret

LoadGDT:
    [bits 16]
    ; load gdt
    lgdt [g_gdtDescriptor]
    ret

KbdControllerDataPort               equ 0x60
KbdControllerCommandPort            equ 0x64
KbdControllerDisableKeyboard        equ 0xAD
KbdControllerEnableKeyboard         equ 0xAE
KbdControllerReadControlOutputPort  equ 0xD0
KbdControllerWriteControlOutputPort equ 0xD1

ScreenBuffer                        equ 0xB8000

g_gdt:
    dq 0

    ; 32 bit code segment
    dw 0xFFFF               ; limit to full 32 bits
    dw 0                    ; base bits 0-15
    db 0                    ; base bits 16-23
    db 10011010b            ; access
    db 11001111b            ; granularity
    db 0                    ; base high

    ; 32 bit data segment
    dw 0xFFFF               ; limit to full 32 bits
    dw 0                    ; base bits 0-15
    db 0                    ; base bits 16-23
    db 10010010b            ; access
    db 11001111b            ; granularity
    db 0                    ; base high

    ; 16 bit code segment
    dw 0xFFFF               ; limit
    dw 0                    ; base bits 0-15
    db 0                    ; base bits 16-23
    db 10011010b            ; access
    db 00001111b            ; granularity
    db 0                    ; base high

    ; 16 bit data segment
    dw 0xFFFF               ; limit
    dw 0                    ; base bits 0-15
    db 0                    ; base bits 16-23
    db 10010010b            ; access
    db 00001111b            ; granularity
    db 0                    ; base high

g_gdtDescriptor:
    dw g_gdtDescriptor - g_gdt - 1 ; size of gdt
    dd g_gdt                       ; address of gdt

g_bootDrive: db 0
