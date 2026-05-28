/* -===============================================================================
    Include
   ===============================================================================- */

#include <arch/rv/base.hpp>
#include <arch/rv/cpu.hpp>
#include <std.hpp>

/* -===============================================================================
    Base
   ===============================================================================- */

rv::cpu::extension_mask_t rv::cpu::get_extensions() {
    return 0;
}

const char * rv::cpu::extension_id(extension_mask_t mask) {
    return "rv64i";
}

void rv::cpu::dump() {
    
}
