/* -===============================================================================
    Include
   ===============================================================================- */

#include <arch/rv/interrupt.hpp>
#include <std.hpp>

/* -===============================================================================
    Internals
   ===============================================================================- */

const rv::Context * g_main_context = nullptr;
const rv::Context * g_main_timer_context = nullptr;
rv::Context g_last_context;

extern "C" void g_trap_handler();

/* -===============================================================================
    Interrupt
   ===============================================================================- */

void rv::interrupt::init() {
    // "li t1, (1 << 1) | (1 << 5) | (1 << 9)\n"
    // "csrs sie, t1"
    
    asm volatile(
        "csrw stvec, %0\n" // Set the interrupt address for S-mode
        "csrsi sstatus, 2\n" // Set the S-level interrupt enable flag (SIE)
        :: "r"(g_trap_handler), "r"(&g_last_context)
        : "memory"
    );
    asm volatile(
        "csrw sscratch, %0"
        :: "r"(&g_last_context)
        : "memory"
    );
    
    kstd::log("interrupts initialized");
}

void rv::interrupt::set_context_handler(const rv::Context * main_context) {
    asm volatile(
        "csrw sscratch, %0"
        :: "r"(&g_last_context)
        : "memory"
    );
    g_main_context = main_context;
}

void rv::interrupt::load_context(const rv::Context * context) {
    asm volatile(
        "csrw sscratch, %0"
        :: "r"(&g_last_context)
        : "memory"
    );
    g_main_context = context;
    sbi::ecall(0, 0);
}

rv::Context & rv::interrupt::last_context() {
    return g_last_context;
}
