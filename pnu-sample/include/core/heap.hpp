#pragma once
#ifndef CORE_HEAP_H
#define CORE_HEAP_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <core/paging.hpp>
#include <std/base.hpp>

/* -===============================================================================
    Constants
   ===============================================================================- */

namespace arch::heap {
    using arch::paging::PAGE_SIZE;
    
    const constexpr size_t SLAB_ENTRY_SIZE = 64;
    const constexpr size_t SLAB_ENTRIES    = PAGE_SIZE / SLAB_ENTRY_SIZE;
}

/* -===============================================================================
    Classes
   ===============================================================================- */

namespace arch::heap {
    // A page with only slab data
    struct alignas(PAGE_SIZE) SlabPage {
        SlabPage() = default;
        
        uint8_t entries[SLAB_ENTRIES][SLAB_ENTRY_SIZE];
    };
    
    // A slab header
    class SlabHeader {
    public:
        SlabHeader() = default;
        SlabHeader(SlabPage * page) : m_page(page) {}
    
        // Tries to allocate a slab within this header
        kstd::Result<uintptr_t, kstd::Status> alloc(size_t count);
        
        // Frees a slab within this header
        void free(uintptr_t ptr, size_t count);
        
        // Returns if a pointer is within this slab
        bool is_within(uintptr_t ptr) const;
    private:
        // Slab entries
        kstd::Bitmap<SLAB_ENTRIES> m_dirty;
        
        // Page this refers to
        SlabPage * m_page = nullptr;
    };
    
    // A page with only slab headers
    struct alignas(PAGE_SIZE) SlabHeaderPage {
        // Tries to allocate a slab within this page
        kstd::Result<uintptr_t, kstd::Status> alloc(size_t count);
        
        // Frees a slab within this header
        void free(uintptr_t ptr, size_t count);
        
        SlabHeader headers[PAGE_SIZE / sizeof(SlabHeader)];
    };
}

/* -===============================================================================
    Functions
   ===============================================================================- */

namespace arch::heap {
    // Allocates raw memory in the heap
    // NOTE: allocating anything >= 4096 will use pages rounded-up to steps of 4096
    kstd::Result<uintptr_t, kstd::Status> alloc_raw(size_t size);
    
    // Frees raw memory from the heap
    void free_raw(uintptr_t ptr, size_t size);

    void init();
}

#endif
