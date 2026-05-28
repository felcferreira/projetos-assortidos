#pragma once
#ifndef CORE_STD_BASE_BIT_H
#define CORE_STD_BASE_BIT_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base/core.hpp>
#include <std/base/mem.hpp>
#include <std/base/container.hpp>
#include <std/base/error.hpp>

/* -===============================================================================
    Bitmap operations
   ===============================================================================- */

namespace kstd::bit {
    // Gets the first free bits given count and alignment
    constexpr size_t find_first_free(Span<const uint8_t> data, size_t count = 1, size_t alignment = 0) {
        if (data.empty()) panic("bitmap is empty");
        const size_t size = data.size() * 8;
        
        size_t max   = 0;
        size_t start = 0;

        for (size_t i = 0; i < size; ++i) {
            // Align before starting
            if (max == 0 && alignment > 1) {
                size_t aligned = align_up(i, alignment);
                if (aligned != i) {
                    i = aligned - 1;
                    continue;
                }
                start = i;
            }
        
            size_t word = data[i / 8];
            size_t mask = (word >> (i % 8)) & 1ULL;

            if (mask) {
                max = 0;
                continue;
            }
            
            if (max == 0)
                start = i;

            ++max;

            if (max == count) return start;
        }

        return SIZE_MAX;
    }
    
    // Returns the amount of set bits
    constexpr size_t set_count(Span<const uint8_t> data) {
        if (data.empty()) panic("bitmap is empty");
        const size_t size = data.size() * 8;
        
        size_t count = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            count += kstd::bit_set_count(data[i]);
        }
        return (count > size ? size : count);
    }
    
    // Returns the amount of zero bits
    constexpr size_t zero_count(Span<const uint8_t> data) {
        if (data.empty()) panic("bitmap is empty");
        const size_t size = data.size() * 8;
        const size_t count = size - set_count(data);
        return (count > size ? size : count);
    }
    
    // Returns if the bitmap is completely empty
    constexpr bool is_empty(Span<const uint8_t> data) {
        if (data.empty()) panic("bitmap is empty");
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] != 0) return false;
        }
        return true;
    }
    
    // Gets a bit at [idx]
    constexpr bool get(Span<const uint8_t> data, size_t idx) {
        if (data.empty()) panic("bitmap is empty");
        if (idx >= data.size() * 8) return false;
        return (data[idx / 8] >> (idx % 8)) & 0x1;
    }
    
    // Sets a bit at [idx] with value [v]
    constexpr void set(Span<uint8_t> data, size_t idx, bool v) {
        if (data.empty()) panic("bitmap is empty");
        if (idx >= data.size() * 8) return;
        
        if (v) data[idx / 8] |= (1 << (idx % 8));
        else   data[idx / 8] &= ~(1 << (idx % 8));
    }

    // Sets the bits [idx] to [idx]+[count] with value [v]
    constexpr void set(Span<uint8_t> data, size_t idx, size_t count, bool v) {
        if (data.empty()) panic("bitmap is empty");
        if (idx + count >= data.size() * 8) return;
        
        const size_t end = idx + count;
        while (idx != end) {
            set(data, idx, v);
            ++idx;
        }
    }
    
    // Dumps all the bits within a span
    constexpr void dump(Span<const uint8_t> data) {
        // if (data.empty()) panic("bitmap is empty");
        for (uint64_t i = 0; i < data.size(); ++i) {
            kstd::log("%d%d%d%d%d%d%d%d", 
                (data[i] & 0x1)  ? 1 : 0, (data[i] & 0x2)  ? 1 : 0,  
                (data[i] & 0x4)  ? 1 : 0, (data[i] & 0x8)  ? 1 : 0, 
                (data[i] & 0x10) ? 1 : 0, (data[i] & 0x20) ? 1 : 0, 
                (data[i] & 0x40) ? 1 : 0, (data[i] & 0x80) ? 1 : 0
            );
        }
    }
}

/* -===============================================================================
    RISCV namespace
   ===============================================================================- */

namespace kstd {
    // A map of bits over a pointer and it's size
    template <typename T = uint8_t>
    class BitmapSpan {
    public:
        static_assert(type::is_same_v<T, uint8_t> || type::is_same_v<T, const uint8_t>, "type must be a byte (uint8_t)");
    
        constexpr BitmapSpan() = default;
        inline BitmapSpan(MemoryRegion region) :
            m_ptr(reinterpret_cast<uint8_t *>(region.start())), m_size(region.size() * 8) {}
        
        template <typename U = void>
        inline BitmapSpan(U * ptr, size_t size) :
            m_ptr(reinterpret_cast<uint8_t *>(ptr)), m_size(size * 8) {}
        
        template <typename U = void, size_t N>
        constexpr BitmapSpan(U (&& data)[N]) :
            m_ptr(data), m_size(N * 8) {}
        
        constexpr BitmapSpan(const BitmapSpan<type::remove_const_t<T>> & other) :
            m_ptr(other.data()), m_size(other.size_bits()) {}
        
        constexpr size_t size_bits() const { return m_size; }
        constexpr size_t size_bytes() const { return m_size / 8; }
    
        constexpr uint8_t * data() { return m_ptr; }
        constexpr const uint8_t * data() const { return m_ptr; }
        
        constexpr auto span() { return Span<uint8_t>(m_ptr, m_size / 8); }
        constexpr auto span() const { return Span<const uint8_t>(m_ptr, m_size / 8); }
        
        // Sets a bit at [idx] with value [v]
        constexpr void set(size_t idx, bool v) { bit::set(span(), idx, v); }
    
        // Sets the bits [idx] to [idx]+[count] with value [v]
        constexpr void set(size_t idx, size_t count, bool v) { bit::set(span(), idx, count, v); }
        
        // Gets a bit at [idx]
        constexpr bool get(size_t idx) const { return bit::get(span(), idx); }
        
        // Gets the last free bits given count and alignment
        constexpr size_t find_first_free(size_t count = 1, size_t alignment = 0) const {
            return bit::find_first_free(span(), count, alignment);
        }
        
        // Returns the amount of set bits
        constexpr size_t set_count() const { return bit::set_count(span()); }
        
        // Returns the amount of zero bits
        constexpr size_t zero_count() const { return bit::zero_count(span()); }
        
        // Returns if the bitmap is completely empty
        constexpr bool is_empty() const { return bit::is_empty(span()); }
        
        constexpr void dump() const { bit::dump(span()); }
    private:
        uint8_t * m_ptr = nullptr;
        size_t m_size = 0;
    };
    
    template <size_t Count>
    class Bitmap {
    public:
        constexpr Bitmap() {
            memset(m_data, 0, Count / 8);
        }
        
        constexpr size_t size_bits() const { return Count; }
        constexpr size_t size_bytes() const { return Count / 8; }
    
        constexpr uint8_t * data() { return m_data; }
        constexpr const uint8_t * data() const { return m_data; }
        
        constexpr auto span() { return Span<uint8_t>(m_data, Count / 8); }
        constexpr auto span() const { return Span<const uint8_t>(m_data, Count / 8); }
        
        // Sets a bit at [idx] with value [v]
        constexpr void set(size_t idx, bool v) { bit::set(span(), idx, v); }
    
        // Sets the bits [idx] to [idx]+[count] with value [v]
        constexpr void set(size_t idx, size_t count, bool v) { bit::set(span(), idx, count, v); }
        
        // Gets a bit at [idx]
        constexpr bool get(size_t idx) const { return bit::get(span(), idx); }
        
        // Gets the last free bits given count and alignment
        constexpr size_t find_first_free(size_t count = 1, size_t alignment = 0) const {
            return bit::find_first_free(span(), count, alignment);
        }
        
        // Returns the amount of set bits
        constexpr size_t set_count() const { return bit::set_count(span()); }
        
        // Returns the amount of zero bits
        constexpr size_t zero_count() const { return bit::zero_count(span()); }
        
        // Returns if the bitmap is completely empty
        constexpr bool is_empty() const { return bit::is_empty(span()); }
        
        constexpr void dump() { bit::dump(span()); }
    private:
        uint8_t m_data[Count / 8];
    };
}

#endif
