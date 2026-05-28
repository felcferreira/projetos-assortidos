#pragma once
#ifndef CORE_RV_INTERRUPT_H
#define CORE_RV_INTERRUPT_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base.hpp>
#include <arch/rv/base.hpp>
#include <arch/rv/sbi.hpp>

/* -===============================================================================
    Context
   ===============================================================================- */

namespace rv::interrupt {
    // Initializes the interrupt driver
    void init();
    
    // Sets the context to use after trapping (main) and before trapping (last)
    void set_context_handler(const rv::Context * main_context);
    
    // Sets a timer that jumps to the main context after trapping
    extern "C" size_t set_timer(size_t count, const rv::Context * context);
    
    // Loads a context
    void load_context(const rv::Context * context);
    
    // The last context before trapping
    rv::Context & last_context();
}

#endif
