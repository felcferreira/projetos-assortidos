OUTPUT_ARCH(riscv)
ENTRY(_start)

/* -===============================================================================
    Constants
   ===============================================================================- */

KERNEL_BASE = 0x80200000; /* The kernel entry point provided by OpenSBI starts at this address */
STACK_SIZE  = 16K;        /* Stack size */
PAGE_SIZE   = 4096;       /* Page size */

SECTIONS
{
    . = KERNEL_BASE;
    
    PROVIDE(LINKER_KERNEL_START = .);
    
    /* Code section */
    .text : ALIGN(PAGE_SIZE) {
        PROVIDE(LINKER_TEXT_START = .);
        *(.text.entry)
        *(.text*)
        PROVIDE(LINKER_TEXT_END = .);
    }

    /* Read-only data */
    .rodata : ALIGN(16) {
        PROVIDE(LINKER_RODATA_START = .);
        *(.rodata*)
        PROVIDE(LINKER_RODATA_END = .);
    }

    /* Runtime data */
    .data : ALIGN(16) {
        PROVIDE(LINKER_DATA_START = .);
        *(.data*)
        PROVIDE(LINKER_DATA_END = .);
    }

    /* Unitialized data */
    .bss : ALIGN(16) {
        PROVIDE(LINKER_BSS_START = .);
        *(.bss*)
        *(COMMON)
        PROVIDE(LINKER_BSS_END = .);
    }

    /* Kernel stack */
    . = ALIGN(16);
    PROVIDE(LINKER_STACK_BOTTOM = .);
    . = . + STACK_SIZE;
    PROVIDE(LINKER_STACK_TOP = .);
    
    /* Kernel end */
    PROVIDE(LINKER_KERNEL_END = .);
}
