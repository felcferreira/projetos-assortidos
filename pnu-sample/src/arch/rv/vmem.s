.global vmem_enable
vmem_enable:
    # PPN
    srli a0, a0, 12
    
    # Sets the mode as Sv39
    li t1, (8 << 60)
    or a0, a0, t1
    
    # Sets the mode to Sv39
    csrw satp, a0
    sfence.vma x0, x0

    ret

.global vmem_disable
vmem_disable:
    # PPN
    li t0, (8 << 60)
    
    # Disables Sv39
    csrc satp, t0
    sfence.vma x0, x0

    ret
