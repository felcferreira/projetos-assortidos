#pragma once
#ifndef CORE_STD_ALLOCATOR_H
#define CORE_STD_ALLOCATOR_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base.hpp>
#include <core/heap.hpp>

/* -===============================================================================
    Allocator structs
   ===============================================================================- */

namespace kstd {
    // The header of an allocation
    struct alignas(16) AllocatorHeader {
        size_t count = 0;
        void (* destructor_fn)(size_t, void *);
    };
}

/* -===============================================================================
    Functions
   ===============================================================================- */

namespace kstd {
    template <typename T>
    Result<T *, Status> alloc(size_t count = 1) {
        auto res = arch::heap::alloc_raw(sizeof(AllocatorHeader) + sizeof(T) * count);
        if (res.error()) return res.unwrap_error();
        
        auto * header = raw_ptr<AllocatorHeader>(res.unwrap());
        header->count = count;
        header->destructor_fn = nullptr;
        if constexpr (!type::is_trivially_destructible_v<T>)
            header->destructor_fn = [](size_t count, void * ptr){
                auto * data = byte_cast<T>(ptr);
                for (size_t i = count; i > 0; --i) {
                    data[i - 1].~T();
                }
            };
        
        return raw_ptr<T>(raw_address(header) + sizeof(AllocatorHeader));
    }
    
    template <typename T>
    Result<T *, Status> alloc(T v, size_t count = 1) {
        auto res = alloc<T>(count);
        if (res.error()) return res.unwrap_error();
        for (size_t i = 0; i < count; ++i) {
            res.unwrap()[i] = v;
        }
    }
    
    template <typename T>
    void free(T * ptr) {
        auto * header = raw_ptr<AllocatorHeader>(raw_address(ptr) - sizeof(AllocatorHeader));
        if (header->destructor_fn) header->destructor_fn(header->count, ptr);
        arch::heap::free_raw(raw_address(header), header->count * sizeof(T));
    }
}

/* -===============================================================================
    Structs
   ===============================================================================- */
   
namespace kstd {
    template <typename T>
    class Box {
    public:
        ~Box() {
            
        }
    private:
        
    };
}

#endif
