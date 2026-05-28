#pragma once
#ifndef KERNEL_LINKER_H
#define KERNEL_LINKER_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std.hpp>

/* -===============================================================================
    Constants
   ===============================================================================- */

// Memory region of the entire kernel
extern uint8_t LINKER_KERNEL_START[];
extern uint8_t LINKER_KERNEL_END[];
extern uint8_t LINKER_KERNEL_SIZE[];

// .text
extern uint8_t LINKER_TEXT_START[];
extern uint8_t LINKER_TEXT_END[];

// .rodata
extern uint8_t LINKER_RODATA_START[];
extern uint8_t LINKER_RODATA_END[];

// .data
extern uint8_t LINKER_DATA_START[];
extern uint8_t LINKER_DATA_END[];

// .bss
extern uint8_t LINKER_BSS_START[];
extern uint8_t LINKER_BSS_END[];

// Heap region, check util/mem.h
extern uint8_t LINKER_HEAP_START[];
extern uint8_t LINKER_HEAP_END[];

// Stack region
extern uint8_t LINKER_STACK_TOP[];
extern uint8_t LINKER_STACK_BOTTOM[];

/* -===============================================================================
    Constants
   ===============================================================================- */

namespace kernel::linker {
    constexpr kstd::MemoryRegion kernel() {
        return kstd::MemoryRegion(LINKER_KERNEL_START, LINKER_KERNEL_END);
    }
    
    constexpr kstd::MemoryRegion text() {
        return kstd::MemoryRegion(LINKER_TEXT_START, LINKER_TEXT_END);
    }
    
    constexpr kstd::MemoryRegion rodata() {
        return kstd::MemoryRegion(LINKER_RODATA_START, LINKER_RODATA_END);
    }
    
    constexpr kstd::MemoryRegion data() {
        return kstd::MemoryRegion(LINKER_DATA_START, LINKER_DATA_END);
    }
    
    constexpr kstd::MemoryRegion bss() {
        return kstd::MemoryRegion(LINKER_BSS_START, LINKER_BSS_END);
    }
    
    constexpr kstd::MemoryRegion heap() {
        return kstd::MemoryRegion(LINKER_HEAP_START, LINKER_HEAP_END);
    }
    
    constexpr kstd::MemoryRegion stack() {
        return kstd::MemoryRegion(LINKER_STACK_BOTTOM, LINKER_STACK_TOP);
    }
}

#endif
