
OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x1000;
    __kernel_start = .;

    .text ALIGN(4K) :
    {
        *(.text*)
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    .data ALIGN(4K) :
    {
        *(.data*)
    }

    /* Reserve 0x7C00–0x7DFF for the bootloader and its GDT. */
    . = 0x8000;

    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        __bss_end = .;
    }

    . = ALIGN(4K);
    __kernel_end = .;

    /* Kernel heap: 64 KiB */
    __heap_start = 0x20000;
    __heap_end = 0x30000;

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
