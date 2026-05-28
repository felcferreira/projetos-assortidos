# Trap handler
.global g_trap_handler
.align 4
g_trap_handler:
    # Saves the context immediately upon entry
    save_context_start:
        # Swaps t0 with sscratch, so we get the stack inside t0
        csrrw t0, sscratch, t0
        
        sd t1,  5*8(t0)
        sd t2,  6*8(t0)
        sd t3,  7*8(t0)
        sd t4,  8*8(t0)
        sd t5,  9*8(t0)
        sd t6, 10*8(t0)
        
        # Reads sscratch (getting the original value of t0 back), then place it in t0
        csrr t1, sscratch
        sd   t1, 4*8(t0)
        
        # Gives sscratch back
        csrw sscratch, t0
        
        # Saves the rest of the context into sscratch
        sd ra, 0*8(t0)
        sd sp, 1*8(t0)
        sd gp, 2*8(t0)
        sd tp, 3*8(t0)
        
        sd s0,  11*8(t0)
        sd s1,  12*8(t0)
        sd s2,  13*8(t0)
        sd s3,  14*8(t0)
        sd s4,  15*8(t0)
        sd s5,  16*8(t0)
        sd s6,  17*8(t0)
        sd s7,  18*8(t0)
        sd s8,  19*8(t0)
        sd s9,  20*8(t0)
        sd s10, 21*8(t0)
        sd s11, 22*8(t0)
        
        sd a0, 23*8(t0)
        sd a1, 24*8(t0)
        sd a2, 25*8(t0)
        sd a3, 26*8(t0)
        sd a4, 27*8(t0)
        sd a5, 28*8(t0)
        sd a6, 29*8(t0)
        sd a7, 30*8(t0)
        
        # Gives sscratch back
        # csrrw t0, sscratch, t0
    save_context_end:
    
    # If the timer triggered the trap, jump to the timer main context
    # else, jump to the normal main context
    timer_check_start:
        # Checkes if scause is related to the timer, disable if true
        csrr t1, scause
        li   t2, 0x8000000000000005 # S-mode timer interrupt
        bne  t1, t2, timer_check_else
        
        # Disables the timer
        li   t2, (1 << 5) # STIE bit
        csrc sie, t2
        
        # Jumps to the main timer context
        la t2, g_main_timer_context
        ld t6, 0(t2)
        
        j timer_check_end
    timer_check_else:
        # Jumps to the main context
        la t2, g_main_context
        ld t6, 0(t2)
    timer_check_end:
    
    # Saves the S-mode context and finalizes the saving
    save_context_post_start:
        # Saves the previous PC
        csrr t1, sepc
        sd   t1, 31*8(t0)
        
        # Saves the cause of the interrupt
        csrr t1, scause
        sd   t1, 32*8(t0)
        
        # Saves the value that comes with the interrupt cause
        csrr t1, stval
        sd   t1, 33*8(t0)
    save_context_post_end:
    
    # Loads the next context
    load_context_start:
        # Load the main context
        mv t0, t6
        
        # Restore sepc from new context
        ld t1, 31*8(t0)
        csrw sepc, t1
        
        # Set return mode (1 = S-mode, 0 = U-mode)
        ld   t3, 34*8(t0)
        csrr t1, sstatus
        andi t1, t1, ~(1 << 8)   # Clear SPP
        slli t3, t3, 8           # Move t2 into bit #8
        or   t1, t1, t3          # Set SPP accordingly
        csrw sstatus, t1
    
        # Restore general registers
        ld ra,   0*8(t0)
        ld sp,   1*8(t0)
        ld gp,   2*8(t0)
        ld tp,   3*8(t0)
    
        ld t1,   5*8(t0)
        ld t2,   6*8(t0)
        ld t3,   7*8(t0)
        ld t4,   8*8(t0)
        ld t5,   9*8(t0)
        ld t6,  10*8(t0)
    
        ld s0,  11*8(t0)
        ld s1,  12*8(t0)
        ld s2,  13*8(t0)
        ld s3,  14*8(t0)
        ld s4,  15*8(t0)
        ld s5,  16*8(t0)
        ld s6,  17*8(t0)
        ld s7,  18*8(t0)
        ld s8,  19*8(t0)
        ld s9,  20*8(t0)
        ld s10, 21*8(t0)
        ld s11, 22*8(t0)
    
        ld a0,  23*8(t0)
        ld a1,  24*8(t0)
        ld a2,  25*8(t0)
        ld a3,  26*8(t0)
        ld a4,  27*8(t0)
        ld a5,  28*8(t0)
        ld a6,  29*8(t0)
        ld a7,  30*8(t0)
        
        # Restore t0 last
        ld t0, 4*8(t0)
    load_context_end:
    
    sret

# Sets the timer
.global set_timer
.align 4
set_timer:
    csrsi sstatus, 2

    # Sets the main timer context
    la t1, g_main_timer_context
    sd a1, 0(t1)
    
    # Get the current time
    rdtime t0
    
    # Calculate the time in the future
    add a0, t0, a0
    
    li a7, 0x54494D45
    li a6, 0x00
    ecall
    
    li t1, (1 << 1) | (1 << 5) | (1 << 9)
    csrs sie, t1
    
    ret
