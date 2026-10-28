.bss:
diskNum: resb 0

.text:
[org 0x7c00]

mov [diskNum], dl

xor ax, ax                          
mov es, ax
mov ds, ax
mov bp, 0x8000
mov sp, bp

mov bx, 0x7e00


mov ah, 2
mov al, 1
mov ch, 0
mov dh, 0
mov cl, 2
mov dl, [diskNum]
int 0x13

mov ah, 0x0e
mov al, [0x7e00]
int 0x10


jmp $
zero: db 0
times 510-($-$$) db 0
db 0x55, 0xaa
times 512 db 'A'