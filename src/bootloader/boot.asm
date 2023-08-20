org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

;
; FAT12 Header
;
jmp short start
nop

bdb_oem:                        db "MSWIN4.1"         ; OEM name 
bdb_bytes_per_sector:           dw 512
bdb_sectors_per_cluster:        db 1
bdb_reserved_sectors:           dw 1
bdb_number_of_fats:             db 2
bdb_dir_entries_count:          dw 0E0h
bdb_total_sectors:              dw 2880               ; 1.44MB
bdb_media_descriptor_type:      db 0F0h
bdb_sectors_per_fat:            dw 9
bdb_sectors_per_track:          dw 18
bdb_number_of_heads:            dw 2
bdb_hidden_sectors:             dd 0
bdb_large_sector_count:         dd 0

;
; extended boot record
;
ebr_drive_number:               db 0
                                db 0                  ; reserved
ebr_signature:                  db 29h
ebr_volume_id:                  dd 12h, 34h, 56h, 78h
ebr_volume_label:               db "DIOS      "      ; 11 bytes
ebr_system_id:                  db "FAT12   "         ; 8 bytes 


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

    ; read from disk
    ; BIOS puts the boot drive number in dl
    mov [ebr_drive_number], dl

    mov ax, 1                          ; LBA address
    mov cl, 1                          ; number of sectors
    mov bx, 0x7E00                     ; address after boot sector
    call disk_read

    ; print hello world
    mov si, msg_hello
    call puts

    cli                                ; disable interrupts
    hlt

;
; Error handling
;
floppy_error:
    mov si, msg_read_failed
    call puts

    jmp key_reboot

key_reboot:
    mov ah, 0
    mov si, msg_keypress
    call puts
    int 0x16                           ; wait for key press
    jmp 0FFFFh:0                       ; jump to start of BIOS

.halt:
    cli                                ; disable interrupts
    jmp .halt

;
; Manipulating data on the Disk
;


;
; Converts logical sector number to CHS
; Input:
;   - ax: logical sector number
; Output:
;   - cx[0, 5]: sector
;   - cx[6, 15]: cylinder
;   - dx: head
;
lba_to_chs:

    push ax
    push dx

    xor dx, dx                         ; dx = 0
    div word [bdb_sectors_per_track]   ; ax = LBA/sectors_per_track
                                       ; dx = LBA%sectors_per_track
    inc dx
    mov cx, dx                         ; cx = sector

    xor dx, dx                         ; dx = 0
    div word [bdb_number_of_heads]     ; ax = LBA/number_of_heads
                                       ; dx = LBA%number_of_heads
    mov dh, dl                         ; dh = head
    mov ch, al                         ; ch = cylinder
    shl ah, 6
    or cl, ah                          ; cx = sector[0, 5] | cylinder[6, 15]

    pop ax
    mov dl, al
    pop ax
    ret

;
; Read a sector from the disk
; Input:
;  -ax: LBA Address
;  -cl: number of sectors (1-128)
;  -dl: drive number
;  -es:bx: Buffer address
disk_read:

    push ax
    push bx
    push cx
    push dx
    push di

    push cx
    call lba_to_chs
    pop ax

    mov ah, 02h
    mov di, 3                          ; retry count

.retry:
    pusha                              ; save registers
    stc                                ; set carry flag
    int 13h                            ; carry flag cleared = success
    jnc .done

    ;read failed
    popa                               ; restore registers
    call disk_reset                    ; reset disk
    dec di                             ; decrement retry count
    test di, di
    jnz .retry

.error:
    jmp floppy_error

.done:
    popa

    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret

;
; Reset the disk
; Input:
;  -dl: drive number
;
disk_reset:
    pusha

    mov ah, 0
    stc
    int 13h
    jc floppy_error

    popa
    ret

msg_hello: db "Hello, World!", ENDL, 0
msg_read_failed: db "Read from disk failed!", ENDL, 0
msg_keypress: db "Press any key to reboot...", ENDL, 0

; boot sector padding
times 510-($-$$) db 0
dw 0xAA55
