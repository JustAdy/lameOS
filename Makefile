CC      := i686-elf-gcc
LD      := i686-elf-ld
NASM    := nasm
QEMU    := qemu-system-x86_64

OBJCOPY := i686-elf-objcopy

SRC_DIR   := src
BUILD_DIR := build

CFLAGS  := -ffreestanding -m32 -g -I$(SRC_DIR) -MMD -MP -masm=intel
KERNEL_ADDR := 0x1000

# Discover C files recursively.
C_SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')
C_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRCS))

# Kernel objects, including assembly files in their current locations.
KERNEL_OBJS := $(BUILD_DIR)/kernel_entry.o $(C_OBJS) $(BUILD_DIR)/idt_load.o $(BUILD_DIR)/idt_stubs.o

.PHONY: all run clean

all: $(BUILD_DIR)/os.img

$(BUILD_DIR):
	mkdir -p $@

# Compile C files recursively, creating matching build subdirectories.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel_entry.o: $(SRC_DIR)/bootloader/kernel_entry.asm | $(BUILD_DIR)
	$(NASM) $< -f elf -o $@

$(BUILD_DIR)/idt_load.o: $(SRC_DIR)/interupts/idt_load.asm | $(BUILD_DIR)
	$(NASM) $< -f elf -o $@

$(BUILD_DIR)/idt_stubs.o: $(SRC_DIR)/interupts/idt_stubs.asm | $(BUILD_DIR)
	$(NASM) $< -f elf -o $@

$(BUILD_DIR)/full_kernel.elf: $(KERNEL_OBJS) $(SRC_DIR)/linker.ld
	$(LD) -T $(SRC_DIR)/linker.ld -o $@ $(KERNEL_OBJS)

$(BUILD_DIR)/full_kernel.bin: $(BUILD_DIR)/full_kernel.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/bootloader/bootloader.asm | $(BUILD_DIR)
	$(NASM) $< -f bin -o $@

$(BUILD_DIR)/zeroes.bin: $(SRC_DIR)/bootloader/zeroes.asm | $(BUILD_DIR)
	$(NASM) $< -f bin -o $@

$(BUILD_DIR)/os.bin: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/full_kernel.bin $(BUILD_DIR)/zeroes.bin
	cat $^ > $@

$(BUILD_DIR)/os.img: $(BUILD_DIR)/os.bin
	dd if=/dev/zero of=$@ bs=1M count=32
	dd if=$< of=$@ conv=notrunc


run: $(BUILD_DIR)/os.img
	$(QEMU) -cpu qemu32 -drive format=raw,file=$<,index=0,media=disk -m 128M

clean:
	rm -rf $(BUILD_DIR)

# Include dependency files from all discovered C objects.
-include $(C_OBJS:.o=.d)