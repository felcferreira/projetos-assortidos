.extern LINKER_STACK_TOP
.extern LINKER_BSS_START
.extern LINKER_BSS_END

.section .text.entry
.global _start
.global trap_handler

_start:
    # Save hart ID and DTB pointer passed by firmware
    mv s0, a0
    mv s1, a1
    
    # Sets the stack pointer to the constant defined by linker.ld
    la sp, LINKER_STACK_TOP

    # Sets the temporary registers to the BSS linker defined constants
    la t0, LINKER_BSS_START
    la t1, LINKER_BSS_END
    
    # Clears the BSS section
    bss_clear_start:
        bge  t0,   t1, bss_clear_end
        sd   zero, 0(t0)
        addi t0,   t0, 8
        j bss_clear_start
    bss_clear_end:
    
    # Jumps to the kernel main function and saves DTB and the hart ID
    mv   a0, s0
    mv   a1, s1
    call kernel_main
    
    # Halts forever
    hang:
        j hang
