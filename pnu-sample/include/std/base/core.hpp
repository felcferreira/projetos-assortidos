#pragma once
#ifndef CORE_STD_BASE_CORE_H
#define CORE_STD_BASE_CORE_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* -===============================================================================
    Util macros
   ===============================================================================- */

#define lenof(__v) (sizeof(__v) / sizeof(* __v))

#define NO_COPY(__type)\
        constexpr __type(const __type &) { static_assert(false, "copy is not allowed for " #__type); }\
        constexpr __type & operator=(const __type &) { static_assert(false, "copy is not allowed for " #__type); return * this; }
#define NO_MOVE(__type)\
        constexpr __type(const __type &&) { static_assert(false, "move is not allowed for " #__type); }\
        constexpr __type && operator=(const __type &&) { static_assert(false, "move is not allowed for " #__type); return * this; }

#define ASM(...) asm volatile(__VA_ARGS__)

/* -===============================================================================
    Kernel standard
   ===============================================================================- */

namespace kstd {
    template <typename T, typename U = uint8_t>
    constexpr U * byte_cast(T * v) { return reinterpret_cast<U *>(v); }
    
    template <typename T, typename U = uint8_t>
    constexpr const U * byte_cast(const T * v) { return reinterpret_cast<const U *>(v); }
    
    template <typename T = uint8_t>
    constexpr T * raw_ptr(uintptr_t addr) { return reinterpret_cast<T *>(addr); }
    
    template <typename T = void>
    constexpr uintptr_t raw_address(T * addr) { return reinterpret_cast<uintptr_t>(addr); }
    
    // Turns any value into a number
    inline uint64_t hash(const void * data, size_t size) {
        uint64_t hash = 0xcbf29ce484222325;
        
        const uint8_t * i = reinterpret_cast<const uint8_t *>(data);
        const uint8_t * end = i + size;
        while (i != end) {
            hash ^= * i;
            hash *= 0x100000001b3;
            ++i;
        }
        
        return hash; 
    }
    
    // Turns any value into a number
    template <typename T>
    constexpr uint64_t hash(const T & data) {
        return hash(&data, sizeof(data)); 
    }
    
    // Rounds a value down to the nearest multiple of the alignment provided
    constexpr uint64_t align_down(uint64_t value, uint64_t align) {
        if (align == 0) return value;
        return (value & ~(align - 1ULL));
    }
    
    // Rounds a value up to the nearest multiple of the alignment provided
    constexpr uint64_t align_up(uint64_t value, uint64_t align) {
        if (align == 0) return value;
        return ((value + (align - 1ULL)) & ~(align - 1ULL));
    }
    
    // Sets the right-most bits given a count
    constexpr uint64_t bit_set_first(uint64_t count) {
        if (count >= 64) return UINT64_MAX;
        return ~(UINT64_MAX << count);
    }
    
    // Sets the left-most bits given a count
    constexpr uint64_t bit_set_last(uint64_t count) {
        if (count >= 64) return UINT64_MAX;
        return ~(UINT64_MAX >> count);
    }
    
    constexpr uint64_t bit_count_leading(uint64_t v) { return __builtin_clzll(v); }
    constexpr uint64_t bit_count_trailing(uint64_t v) { return __builtin_ctzll(v); }
    constexpr uint64_t bit_count_leading_sign_bits(uint64_t v) { return __builtin_clrsbll(v); }
    constexpr uint64_t bit_set_count(uint64_t v) { return __builtin_popcount(v); }
    constexpr uint64_t bit_zero_count(uint64_t v) { return 64 - __builtin_popcount(v); }
    constexpr uint64_t bit_parity(uint64_t v) { return __builtin_parity(v); }
}

#endif
