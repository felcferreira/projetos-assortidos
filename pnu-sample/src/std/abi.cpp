/* -===============================================================================
    Include
   ===============================================================================- */

#include <std/base/core.hpp>
#include <std/base/error.hpp>

/* -===============================================================================
    C++ ABI
   ===============================================================================- */
   
// extern "C" void * memset(void * ptr, int value, size_t size) { return __builtin_memset(ptr, value, size); }
// extern "C" int memcmp(const void * s1, const void * s2, size_t n) { return __builtin_memcmp(s1, s2, n); }
// extern "C" void * memmove(void * dst, const void * src, size_t len) { return __builtin_memmove(dst, src, len); }
// extern "C" void * memchr(const void * s, int c, size_t n) { return __builtin_memchr(s, c, n); }
// extern "C" void memcpy(void * dst, const void * src, size_t size) { __builtin_memcpy(dst, src, size); }

extern "C" void * memset(void * ptr, int value, size_t size) {
    unsigned char * p = (unsigned char *)ptr;
    unsigned char v = (unsigned char)value;
    for (size_t i = 0; i < size; ++i)
        p[i] = v;
    return ptr;
}

extern "C" int memcmp(const void * s1, const void * s2, size_t n) {
    const unsigned char * p1 = (const unsigned char *)s1;
    const unsigned char * p2 = (const unsigned char *)s2;

    for (size_t i = 0; i < n; ++i) {
        if (p1[i] != p2[i])
            return (int)p1[i] - (int)p2[i];
    }
    return 0;
}

extern "C" void * memcpy(void * dst, const void * src, size_t size) {
    unsigned char * d = (unsigned char *)dst;
    const unsigned char * s = (const unsigned char *)src;

    for (size_t i = 0; i < size; ++i)
        d[i] = s[i];

    return dst;
}

extern "C" void * memmove(void * dst, const void * src, size_t len) {
    unsigned char * d = (unsigned char *)dst;
    const unsigned char * s = (const unsigned char *)src;

    if (d < s) {
        for (size_t i = 0; i < len; ++i)
            d[i] = s[i];
    } else if (d > s) {
        for (size_t i = len; i-- > 0;)
            d[i] = s[i];
    }

    return dst;
}

extern "C" void * memchr(const void * s, int c, size_t n) {
    const unsigned char * p = (const unsigned char *)s;
    unsigned char v = (unsigned char)c;

    for (size_t i = 0; i < n; ++i) {
        if (p[i] == v)
            return (void*)(p + i);
    }
    return nullptr;
}

void * __dso_handle = nullptr;

extern "C" int __cxa_atexit(void (*)(void *), void *, void *) {
    return 0;
}

extern "C" void __cxa_pure_virtual() {
    kstd::panic("purely virtual function called");
    while (1);
}
