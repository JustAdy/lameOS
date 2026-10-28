[org 0x7c00]
    mov ax, 12345
    call print_number
    jmp $



; Prints the value of ax on the screen 
print_number:
    mov bx, 0
    mov cx, 10
div_loop:
    mov dx, 0
    div cx
    add dl, 48
    mov [buffer + bx], dl
    inc bx
    cmp ax, 0
    jne div_loop
; Print
    mov bx, 5
    mov cl, 0
    mov ah, 0x0e
    mov al, [buffer + bx]
    mov [buffer + bx], cl
print_loop:
    int 0x10
    dec bx
    mov al, [buffer + bx]
    cmp al, 0
    jne print_loop

    ret

db 0
buffer:
    times 5 db 0
times 510-($-$$) db 0
db 0x55, 0xaa