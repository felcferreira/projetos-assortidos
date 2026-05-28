#pragma once
#ifndef CORE_PAGING_H
#define CORE_PAGING_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base.hpp>

/* -===============================================================================
    Constants
   ===============================================================================- */

namespace arch::paging {
    const constexpr size_t PAGE_SIZE = 4096;
}

/* -===============================================================================
    Functions
   ===============================================================================- */

namespace arch::paging {
    // Obtains the memory region 
    extern kstd::Status init(uintptr_t mem_start, uintptr_t mem_end);
    
    // Obtains the total amount of pages
    extern size_t total_pages();
    
    // Obtains the total amount of bytes
    extern size_t total();
    
    // Obtains the total memory region
    extern kstd::MemoryRegion total_memory();
    
    // Obtains the total usable region
    extern kstd::MemoryRegion usable_memory();
    
    // Obtains the mapping region
    extern kstd::MemoryRegion mapping_memory();
    
    // Obtains the reserved region
    extern kstd::MemoryRegion reserved_memory();
    
    // Allocates a page
    extern kstd::Result<size_t, kstd::Status> alloc(size_t count = 1, size_t alignment = 0);
    
    // Frees a page
    extern void free(size_t page_index, size_t count = 1);
    
    // Dumps
    extern void dump();
    
    // Obtains the address of a page
    extern uintptr_t page_to_address(size_t index);
    
    // Obtains the page of an address
    extern size_t address_to_page(uintptr_t ptr);
    
    // Obtains the mapping region
    extern inline kstd::MemoryRegion page_to_region(size_t index, size_t count = 1) {
        return kstd::MemoryRegion(page_to_address(index), page_to_address(index) + count * PAGE_SIZE);
    }
}

#endif
