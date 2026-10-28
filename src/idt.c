#include <system.h>
#include <string.h>
#include <vga_terminal.h>

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;        /* Our kernel segment goes here! */
    unsigned char always0;     /* This will ALWAYS be set to 0! */
    unsigned char flags;       /* Set using the above table! */
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/* Declare an IDT of 256 entries. Although we will only use the
*  first 32 entries in this tutorial, the rest exists as a bit
*  of a trap. If any undefined IDT entry is hit, it normally
*  will cause an "Unhandled Interrupt" exception. Any descriptor
*  for which the 'presence' bit is cleared (0) will generate an
*  "Unhandled Interrupt" exception */
struct idt_entry idt[256];
struct idt_ptr idtp;

/* This exists in 'idt_load.asm', and is used to load our IDT */
extern void idt_load();

/* Use this function to set an entry in the IDT. Alot simpler
*  than twiddling with the GDT ;) */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

#define idt_sel 0x08
#define idt_flags 0x8E

extern void divide_stub();
extern void debug_stub();
extern void nmi_stub();
extern void breakpoint_stub();
extern void overflow_stub();
extern void bound_stub();
extern void opcode_stub();
extern void device_stub();
extern void double_stub();
extern void tss_stub();
extern void segment_stub();
extern void stack_stub();
extern void general_stub();
extern void page_stub();
extern void alignment_stub();

/* Installs the IDT */
void idt_install()
{
    /* Sets the special IDT pointer up*/
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    /* Add any new ISRs to the IDT here using idt_set_gate */
    idt_set_gate(0, (unsigned long)divide_stub, idt_sel, idt_flags);
    idt_set_gate(1, (unsigned long)debug_stub, idt_sel, idt_flags);
    idt_set_gate(2, (unsigned long)nmi_stub, idt_sel, idt_flags);
    idt_set_gate(3, (unsigned long)breakpoint_stub, idt_sel, idt_flags);
    idt_set_gate(4, (unsigned long)overflow_stub, idt_sel, idt_flags);
    idt_set_gate(5, (unsigned long)bound_stub, idt_sel, idt_flags);
    idt_set_gate(6, (unsigned long)opcode_stub, idt_sel, idt_flags);
    idt_set_gate(7, (unsigned long)device_stub, idt_sel, idt_flags);
    idt_set_gate(8, (unsigned long)double_stub, idt_sel, idt_flags);
    idt_set_gate(10, (unsigned long)tss_stub, idt_sel, idt_flags);
    idt_set_gate(11, (unsigned long)segment_stub, idt_sel, idt_flags);
    idt_set_gate(12, (unsigned long)stack_stub, idt_sel, idt_flags);
    idt_set_gate(13, (unsigned long)general_stub, idt_sel, idt_flags);
    idt_set_gate(14, (unsigned long)page_stub, idt_sel, idt_flags);
    idt_set_gate(17, (unsigned long)alignment_stub, idt_sel, idt_flags);


    /* Points the processor's internal register to the new IDT */
    idt_load();
}

struct stack_frame
{
    unsigned long edi;
    unsigned long esi;
    unsigned long esp;
    unsigned long ebx;
    unsigned long ebp;
    unsigned long edx;
    unsigned long ecx;
    unsigned long eax;
    unsigned long vector;
    unsigned long error_code;
    unsigned long eip;
    unsigned long cs;
    unsigned long eflags;
} __attribute__((packed));

void ISR(struct stack_frame *frame)
{
    unsigned long instruction = frame->eip;
    print_string("Error occurred while executing instruction at address: ");
    print_long(instruction);
    put_char('\n');
    print_long(frame->vector);
    put_char('\t');
    print_long(frame->error_code);

    asm volatile ("cli");
    for (;;)
        asm volatile ("hlt");
    
}
