bits 16

section _TEXT class=CODE

;
;void _cdecl x86_div64_32(uint64_t dividend, uint32_t divisor, uint64_t *quotient, uint64_t *remainder)
;
global _x86_div64_32
_x86_div64_32:

	; call frame
	push bp
	mov bp, sp

	; divide upper 32 bits

	; restore frame
	mov sp, bp
	pop bp
	ret

;
; void _cdecl x86_Video_WriteCharTeletype(char c, uint8_t page);
;
global _x86_Video_WriteCharTeletype
_x86_Video_WriteCharTeletype:
		
	; call frame
	push bp
	mov bp, sp

	; save bx
	push bx

	; [bp+0] - old call frame
	; [bp+2] - return address
	; [bp+4] - character
	; [bp+6] - page
	mov ah, 0Eh
	mov al, [bp+4]
	mov bh, [bp+6]

	int 10h

	; restore bx
	pop bx

	; restore frame
	mov sp, bp
	pop bp
	ret
