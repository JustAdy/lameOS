i686-elf-gcc -ffreestanding -m32 -g -c "kernel.c" -o "kernel.o"
nasm "kernel_entry.asm" -f elf -o "kernel_entry.o"
i686-elf-ld -o "full_kernel.bin" -Ttext 0x1000 "kernel_entry.o" "kernel.o" --oformat binary
nasm "bootloader.asm" -f bin -o "boot.bin"
cat "boot.bin" "full_kernel.bin" > "everything.bin"
nasm "zeroes.asm" -f bin -o "zeroes.bin"
cat "everything.bin" "zeroes.bin" > "os.bin"
qemu-system-x86_64 -drive format=raw,file="os.bin",index=0,if=floppy -m 128M