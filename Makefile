CC      := i686-elf-gcc
LD      := i686-elf-ld
NASM    := nasm
QEMU    := qemu-system-x86_64

SRC_DIR   := src
BUILD_DIR := build

CFLAGS  := -ffreestanding -m32 -g -I$(SRC_DIR) -MMD -MP
KERNEL_ADDR := 0x1000

C_SRCS := $(wildcard $(SRC_DIR)/*.c)
C_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRCS))

# kernel_entry.o must come first so it lands at the kernel's load address
KERNEL_OBJS := $(BUILD_DIR)/kernel_entry.o $(C_OBJS) $(BUILD_DIR)/idt_load.o $(BUILD_DIR)/idt_stubs.o

.PHONY: all run clean

all: $(BUILD_DIR)/os.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compiles every src/*.c (kernel.c, vga_terminal.c, ...); -MMD tracks header deps
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel_entry.o: $(SRC_DIR)/kernel_entry.asm | $(BUILD_DIR)
	$(NASM) $< -f elf -o $@

$(BUILD_DIR)/idt_load.o: $(SRC_DIR)/idt_load.asm | $(BUILD_DIR)
	$(NASM) $< -f elf -o $@

$(BUILD_DIR)/idt_stubs.o: $(SRC_DIR)/idt_stubs.asm | $(BUILD_DIR)
	$(NASM) $< -f elf -o $@

$(BUILD_DIR)/full_kernel.bin: $(KERNEL_OBJS)
	$(LD) -o $@ -Ttext $(KERNEL_ADDR) $^ --oformat binary

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/bootloader.asm | $(BUILD_DIR)
	$(NASM) $< -f bin -o $@

$(BUILD_DIR)/zeroes.bin: $(SRC_DIR)/zeroes.asm | $(BUILD_DIR)
	$(NASM) $< -f bin -o $@

$(BUILD_DIR)/os.bin: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/full_kernel.bin $(BUILD_DIR)/zeroes.bin
	cat $^ > $@

run: $(BUILD_DIR)/os.bin
	$(QEMU) -drive format=raw,file=$<,index=0,if=floppy -m 128M

clean:
	rm -rf $(BUILD_DIR)

-include $(C_OBJS:.o=.d)