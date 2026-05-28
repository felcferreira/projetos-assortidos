debug_hang:
    csrci sstatus, 2
    csrw  sie, zero
    ebreak
debug_hang_loop:
    wfi
    j debug_hang_loop
