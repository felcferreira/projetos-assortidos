#pragma once
#ifndef ARCH_RV_CPU_H
#define ARCH_RV_CPU_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base.hpp>

/* -===============================================================================
    Classes
   ===============================================================================- */

namespace rv::cpu {
    using extension_mask_t = uint64_t;
    
    // Obtains all the extensions of the CPU
    extension_mask_t get_extensions();
    
    // Obtains the ID of an extension
    const char * extension_id(extension_mask_t mask);
    
    // Dumps all the extensions of the CPU
    void dump();
}

#endif
