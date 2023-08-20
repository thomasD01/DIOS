org 0x0
bits 16

%define ENDL 0x0D, 0x0A

start:
    jmp main 

main:

    mov si, msg_hello
    call puts

.halt:
    cli
    hlt

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

msg_hello: db "Hello, World! the bootloader works", ENDL, 0
