#pragma once
#ifndef ARCH_RV_BASE_H
#define ARCH_RV_BASE_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base.hpp>
#include <arch/rv/sbi.hpp>

/* -===============================================================================
    Context
   ===============================================================================- */

namespace rv {
    // Defines a register
    using register_t = uint64_t;
    
    // Base registers to be saved during context switching
    struct ContextBase {
        register_t ra, sp, gp, tp;
    
        register_t t0, t1, t2, t3, t4, t5, t6;
        register_t s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
        register_t a0, a1, a2, a3, a4, a5, a6, a7;
        
        register_t pc;
        register_t cause, val; // Cause and value of the return
        register_t mode;       // S-mode toggle (1 = S-mode, 0 = U-mode)
        
        register_t spc;
    };
    
    // Extended context
    struct Context : ContextBase {
        
        
        register_t flags;
    };
    
    // Debug putc
    void putc(char c);
    
    // Debug puts
    void puts(const char * str, size_t n = 0);
}

#endif
