; Boot sector code offset: 0x7c00

	mov ah, 0x0e
	mov bl, string
	add bx, 0x7c00
print_next:
	mov al,[bx]
	cmp al, 0x0
	je inf_loop
	int 0x10
	add bl, 0x1
	jmp print_next
inf_loop:
	jmp $
string:
	db "Hello World",0
	times 510-($-$$) db 0
	dw 0xaa55
