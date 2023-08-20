org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A
%define SETUP_LOAD_ADDRESS 0x2000
%define SETUP_LOAD_OFFSET 0

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
ebr_volume_label:               db "DIOS       "      ; 11 bytes
ebr_system_id:                  db "FAT12   "         ; 8 bytes 


start:
    ; setup data segments
    mov ax, 0
    mov ds, ax
    mov es, ax

    ; setup stack
    mov ss, ax
    mov sp, 0x7C00

    push es 
    push word .after
    retf

.after:

    ; read from disk
    ; BIOS puts the boot drive number in dl
    mov [ebr_drive_number], dl

    ; loading message
    mov si, msg_loading
    call puts

    ; read drive parameters
    push es
    mov ah, 08h
    int 13h
    jc key_reboot
    pop es

    and cl, 0x3F                       ; cl = sectors per track
    xor ch, ch                         ; ch = 0
    mov [bdb_sectors_per_track], cx

    inc dh                             ; dh = number of heads
    mov [bdb_number_of_heads], dh

    ; calculate LBA address of root directory
    mov ax, [bdb_sectors_per_fat]      
    mov bl, [bdb_number_of_fats]
    xor bh, bh                         ; bh = 0
    mul bx                             ; ax = sectors_per_fat * number_of_fats            
    add ax, [bdb_reserved_sectors]     ; ax = LBA
    push ax

    ; calculate size of root directory
    mov ax, [bdb_sectors_per_fat]
    shl ax, 5                          ; ax = sectors_per_fat * 32
    xor dx, dx                         ; dx = 0
    div word [bdb_bytes_per_sector]    ; ax = number of sectors to read

    test dx, dx                        ; check for remainder
    jz .root_dir_after
    inc ax                             ; ax = number of sectors to read + 1

.root_dir_after:

    ; safe size of root directory
    mov [root_dir_size], ax

    ; read root directory
    mov cl, al                         ; cl = number of sectors to read
    pop ax                             ; ax = LBA
    mov dl, [ebr_drive_number]         ; dl = drive number
    mov bx, buffer                     ; es:bx = buffer address
    call disk_read

    ; search for setup.bin
    xor bx, bx
    mov di, buffer

.search_setup:
    mov si, file_setup_bin
    mov cx, 11
    push di
    repe cmpsb
    pop di
    je .setup_found

    ; next entry
    add di, 32
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .search_setup

    ; setup not found
    jmp key_reboot


.setup_found:

    ; di = address to first cluster
    mov ax, [di + 26]
    mov [setup_cluster], ax

    ; load FAT 
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    ; load setup
    mov bx, SETUP_LOAD_ADDRESS
    mov es, bx
    mov bx, SETUP_LOAD_OFFSET

.load_setup_loop:

    ; calculate offset
    mov ax, [setup_cluster]

    add ax, 31 ; TODO: remove this hack

    ; read cluster
    mov cl, 1
    mov dl, [ebr_drive_number]
    call disk_read

    add bx, [bdb_bytes_per_sector]

    ; next cluster
    mov ax, [setup_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx                             ; ax = cluster * 3 / 2
                                       ; dx = cluster * 3 % 2

    mov si, buffer
    add si, ax
    mov ax, [ds:si]                    ; ax = FAT entry

    or dx, dx
    jz .even 

.odd:
    shr ax, 4
    jmp .next_cluster

.even:
    and ax, 0x0FFF

.next_cluster:
    cmp ax, 0x0FF8                     ; check for end of file
    jae .read_finished

    mov [setup_cluster], ax
    jmp .load_setup_loop

.read_finished:

    ; jump to setup
    mov dl, [ebr_drive_number] ; boot device in dl

    ; set segment registers
    mov ax, SETUP_LOAD_ADDRESS
    mov ds, ax
    mov es, ax

    jmp SETUP_LOAD_ADDRESS:SETUP_LOAD_OFFSET
    
    jmp key_reboot

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
    jmp key_reboot

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
    jc key_reboot

    popa
    ret

key_reboot:
    mov ah, 0
    int 16h

msg_loading:          db "Loading...",                 ENDL, 0
file_setup_bin:       db "SETUP   BIN",                      0
setup_cluster:        dw                                     0
root_dir_size:        dw                                     0

; boot sector padding
times 510-($-$$) db 0
dw 0xAA55

buffer:
