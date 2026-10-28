    [org 0x7c00]
    mov bx, 0
    mov ah, 0x0e
    mov al, [string]
loop:
    int 0x10
    inc bx
    mov al, [string + bx]
    cmp al, 0
    jne loop

type:
    mov ah, 0
    int 0x16
    mov [char], al

    mov ah, 0x0e
    mov al, [char]
    int 0x10
    jmp type

char:
    db 0
string:
    db "This is a very important string.", 0

times 510-($-$$) db 0
db 0x55, 0xaa