
mov ah, 0x0e
mov al, 65

loop:
    int 0x10
    xor al, 0b0010_0000
    inc al
    cmp al, 91
    je end
    jmp loop

end:
jmp $

times 510-($-$$) db 0
db 0x55, 0xaa