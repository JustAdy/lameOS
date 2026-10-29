[org 0x7C00]
KERNEL_LOCATION equ 0x1000

mov [BOOT_DISK], dl

xor ax, ax                          
mov es, ax
mov ds, ax
mov bp, 0x8000
mov sp, bp

mov bx, KERNEL_LOCATION
mov dh, [SECTORS_TO_READ]   ; How many sectors to read

mov ah, 2
mov al, dh
mov ch, 0
mov dh, 0
mov cl, 2
mov dl, [BOOT_DISK]
int 0x13

jnc carry_ok
jmp read_error
carry_ok:
    cmp al, [SECTORS_TO_READ]
    je enter_text

enter_text:
mov ah, 0x0
mov al, 0x3
int 0x10                    ; Text Mode

CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:start_protected_mode

jmp $

read_error:
    mov bx, 0
    mov ah, 0x0e
    mov al, [string]
loop:
    int 0x10
    inc bx
    mov al, [string + bx]
    cmp al, 0
    jne loop
jmp $

GDT_start:                          ; must be at the end of real mode code
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start

[bits 32]
start_protected_mode:
    mov ax, DATA_SEG    ; Set up segment registers and stack
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000
    mov esp, ebp

    jmp KERNEL_LOCATION

string db "Error while reading kernel!", 0
SECTORS_TO_READ db 10
BOOT_DISK: db 0
times 510-($-$$) db 0
dw 0xaa55