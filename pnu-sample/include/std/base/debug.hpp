#pragma once
#ifndef CORE_STD_BASE_DEBUG_H
#define CORE_STD_BASE_DEBUG_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base/core.hpp>
#include <std/base/error.hpp>

/* -===============================================================================
    Kernel standard
   ===============================================================================- */

namespace kstd::debug {
    template <typename T>
    inline const char * type_name() {
        auto find = [](const char * s, char c) {
            while (*s && *s != c) ++s;
            return s;
        };
    
#if defined(__clang__) || defined(__GNUC__)
        constexpr const char * p = __PRETTY_FUNCTION__;
        // "const char * type_name() [T = X]"
        const char * start = find(p, '=');
        start += 2; // skip "= "
        const char * end = find(start, ']');
        static char buffer[128]{};
        size_t i = 0;
        while (start < end && i < 127)
            buffer[i++] = *start++;
        buffer[i] = 0;
        return buffer;
#else
        return "unknown";
#endif
    }

    template <typename T>
    struct Tracked {
        inline Tracked() { kstd::log("%s constructed at %p", type_name<T>(), this); }
        inline ~Tracked() { kstd::log("%s destructed at %p", type_name<T>(), this); }
    };
}

#endif
