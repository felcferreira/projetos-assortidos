#pragma once
#ifndef CORE_STD_BASE_MEM_H
#define CORE_STD_BASE_MEM_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base/core.hpp>
#include <std/base/abi.hpp>
#include <std/base/error.hpp>

/* -===============================================================================
    Classes
   ===============================================================================- */

namespace kstd {
    // A memory region
    class MemoryRegion {
    public:
        constexpr MemoryRegion() = default;
        constexpr MemoryRegion(uintptr_t start, uintptr_t end) :
            m_start(start), m_end(end) {}
        
        template <typename T>
        constexpr MemoryRegion(T * start, T * end) :
            m_start(reinterpret_cast<uintptr_t>(start)), m_end(reinterpret_cast<size_t>(end)) {}
        
        template <typename T, size_t N>
        constexpr MemoryRegion(T * ptr) :
            m_start(reinterpret_cast<uintptr_t>(ptr)), m_end(reinterpret_cast<size_t>(ptr + N)) {}

        // Acesses a unaligned address
        template <typename T>
        constexpr Result<T *, Status> unaligned(uintptr_t addr) {
            if (addr < m_start || addr + sizeof(T) >= m_end) return STATUS_INVALID_ADDRESS;
            return reinterpret_cast<T *>(addr);
        }

        // Acesses a unaligned address
        template <typename T>
        constexpr const Result<const T *, Status> unaligned(uintptr_t addr) const {
            if (addr < m_start || addr + sizeof(T) >= m_end) return STATUS_INVALID_ADDRESS;
            return reinterpret_cast<const T *>(addr);
        }
        
        // Acesses a aligned address
        template <typename T>
        constexpr Result<T *, Status> aligned(uintptr_t addr) {
            if (addr < m_start || addr + sizeof(T) >= m_end) return STATUS_INVALID_ADDRESS;
            if (addr % alignof(T) == 0) return STATUS_UNALIGNED_ACCESS;
            return reinterpret_cast<T *>(addr);
        }
        
        // Acesses a aligned address
        template <typename T>
        constexpr const Result<const T *, Status> aligned(uintptr_t addr) const {
            if (addr < m_start || addr + sizeof(T) >= m_end) return STATUS_INVALID_ADDRESS;
            if (addr % alignof(T) == 0) return STATUS_UNALIGNED_ACCESS;
            return reinterpret_cast<const T *>(addr);
        }
        
        constexpr void set_start(uintptr_t start) { m_start = start; }
        constexpr void set_end(uintptr_t end) { m_end = end; }
        constexpr void set_size(size_t size) {
            if (m_end == 0)
                m_end = m_start + size;
            else
                m_start = m_end - size;
        }
        
        constexpr uintptr_t start() const { return m_start; }
        constexpr uintptr_t end() const { return m_end; }
        constexpr size_t    size() const { return m_end - m_start; }
        
        template <typename T = uint8_t>
        constexpr T * data() { return reinterpret_cast<T *>(m_start); }
        template <typename T = uint8_t>
        constexpr const T * data() const { return reinterpret_cast<const T *>(m_start); }
        
        constexpr bool is_within_range(uintptr_t addr) const {
            return (addr >= m_start && addr < m_end);
        }
    private:
        uintptr_t m_start = 0;
        uintptr_t m_end = 0;
    };
}

#endif
