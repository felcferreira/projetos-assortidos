#pragma once
#ifndef CORE_RV_SBI_H
#define CORE_RV_SBI_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base.hpp>

/* -===============================================================================
    Context
   ===============================================================================- */

namespace rv::sbi {
    constexpr const int EID_CONSOLE = 0x1;
    constexpr const int FID_PUTCHAR = 0;
    constexpr const int FID_GETCHAR = 1;

    struct Ret {
        uint64_t error, value;
    };

    static inline Ret ecall(uint64_t eid, uint64_t fid, uint64_t arg0 = 0, uint64_t arg1 = 0, uint64_t arg2 = 0, uint64_t arg3 = 0, uint64_t arg4 = 0, uint64_t arg5 = 0) {
        register uint64_t a0 asm("a0") = arg0;
        register uint64_t a1 asm("a1") = arg1;
        register uint64_t a2 asm("a2") = arg2;
        register uint64_t a3 asm("a3") = arg3;
        register uint64_t a4 asm("a4") = arg4;
        register uint64_t a5 asm("a5") = arg5;
        register uint64_t a6 asm("a6") = fid;
        register uint64_t a7 asm("a7") = eid;
    
        asm volatile("ecall"
            : "+r"(a0), "+r"(a1)
            : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
            : "memory"
        );
    
        return { a0, a1 };
    }
}

#endif
