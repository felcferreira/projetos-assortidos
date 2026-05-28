#pragma once
#ifndef CORE_STD_BASE_ABI_H
#define CORE_STD_BASE_ABI_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base/core.hpp>

/* -===============================================================================
    C++ ABI
   ===============================================================================- */

extern "C" void __cxa_pure_virtual();

/* -===============================================================================
    C ABI
   ===============================================================================- */

extern "C" void * memset(void * ptr, int value, size_t size);
extern "C" int memcmp(const void * s1, const void * s2, size_t n);
extern "C" void * memmove(void * dst, const void * src, size_t len);
extern "C" void * memchr(const void * s, int c, size_t n);
extern "C" void memcpy(void * dst, const void * src, size_t size);

extern "C" inline int strcmp(const char * s1, const char * s2) {
    while (* s1 && (*s1 == * s2)) {
        ++s1, ++s2;
    }
    return (char)* s1 - (char)* s2;
}

extern "C" inline int strncmp(const char * s1, const char * s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        ++s1, ++s2;
        n--;
    }
    if (n == 0) return 0;
    return (char)* s1 - (char)* s2;
}

extern "C" inline uint64_t strlen(const char * s) {
    const char * p = s;
    while (* p != '\0') ++p;
    return p - s;
}

extern "C" inline char * strchr(const char * s, int c) {
    char ch = (char)c;

    while (* s) {
        if (* s == ch) return (char *)s;
        ++s;
    }
    if (ch == '\0') return (char *)s;
    return NULL;
}

extern "C" inline char * strrchr(const char * s, int c) {
    const char * last = NULL;
    char ch = (char)c;

    while (* s) {
        if (* s == ch) last = s;
        ++s;
    }
    if (ch == '\0') return (char *)s;
    return (char *)last;
}

extern "C" inline uint64_t strnlen(const char * str, uint64_t size) {
    uint64_t i = 0;
    while (* str++ != '\0' && i != size) ++i;
    return i;
}

extern "C" inline void strcpy(char * dst, const char * src) {
    while (* src != '\0') * dst++ = * src++;
}

extern "C" inline uint32_t be32tole(uint32_t x) {
    return ((x & 0x000000FFU) << 24) |
           ((x & 0x0000FF00U) << 8)  |
           ((x & 0x00FF0000U) >> 8)  |
           ((x & 0xFF000000U) >> 24);
}

extern "C" inline uint64_t be64tole(uint64_t x) {
    return ((x & 0x00000000000000FFULL) << 56) |
           ((x & 0x000000000000FF00ULL) << 40) |
           ((x & 0x0000000000FF0000ULL) << 24) |
           ((x & 0x00000000FF000000ULL) << 8)  |
           ((x & 0x000000FF00000000ULL) >> 8)  |
           ((x & 0x0000FF0000000000ULL) >> 24) |
           ((x & 0x00FF000000000000ULL) >> 40) |
           ((x & 0xFF00000000000000ULL) >> 56);
}

#endif
