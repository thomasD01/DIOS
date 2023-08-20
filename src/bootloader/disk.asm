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
