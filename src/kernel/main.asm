org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

start:
    jmp main

;
; Write a string to the screen
; params: ds:si - pointer to string
;
puts:
    push ax
    push bx
    push si

.loop:
    lodsb
    or al, al
    jz .done

    ; write character to screen
    mov ah, 0x0E
    mov bh, 0x00
    int 0x10

    jmp .loop

.done:
    pop si
    pop bx
    pop ax
    ret

main:
    ; setup data segments
    mov ax, 0
    mov ds, ax
    mov es, ax

    ; setup stack
    mov ss, ax
    mov sp, 0x7C00

    hlt

.halt:
    jmp .halt


msg_hello: db "Hello, World!", ENDL, 0

; boot sector padding
times 510-($-$$) db 0
dw 0xAA55
