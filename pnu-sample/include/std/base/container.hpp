#pragma once
#ifndef CORE_STD_BASE_CONTAINER_H
#define CORE_STD_BASE_CONTAINER_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base/core.hpp>
#include <std/base/error.hpp>
#include <std/base/type.hpp>

/* -===============================================================================
    RISCV namespace
   ===============================================================================- */

namespace kstd {
    // View over an array
    template <typename T>
    class Span {
    public:
        constexpr Span() = default;
        constexpr Span(T * data, size_t size) :
            m_data(data), m_size(size) {}
        
        template <size_t N>
        constexpr Span(T data[N]) :
            m_data(data), m_size(N) {}

        constexpr size_t size() const { return m_size; }
        constexpr bool empty() const { return m_size == 0; }

        constexpr T * data() { return m_data; }
        constexpr const T * data() const { return m_data; }
        
        constexpr T & operator[](size_t idx) {
            if (!m_data || m_size == 0) kstd::panic("access on an empty span");
            return m_data[idx];
        }
        
        constexpr const T & operator[](size_t idx) const {
            if (!m_data || m_size == 0) kstd::panic("access on an empty span");
            return m_data[idx];
        }
        
        constexpr T * begin() { return m_data; }
        constexpr const T * begin() const { return m_data; }
        constexpr T * end() { return m_data + m_size; }
        constexpr const T * end() const { return m_data + m_size; }
    private:
        T * m_data = nullptr;
        size_t m_size = 0;
    };
    
    // View over an array (const version)
    template <typename T>
    class Span<const T> {
    public:
        constexpr Span() = default;
        constexpr Span(const T * data, size_t size) :
            m_data(data), m_size(size) {}
        
        template <size_t N>
        constexpr Span(const T data[N]) :
            m_data(data), m_size(N) {}
        
        constexpr Span(Span<T> span) :
            m_data(span.data()), m_size(span.size()) {}

        constexpr size_t size() const { return m_size; }
        constexpr bool empty() const { return m_size == 0; }
        
        constexpr const T * data() const { return m_data; }
        
        constexpr const T & operator[](size_t idx) const {
            if (!m_data || m_size == 0) kstd::panic("access on an empty span");
            return m_data[idx];
        }
        
        constexpr const T * begin() const { return m_data; }
        constexpr const T * end() const { return m_data + m_size; }
    private:
        const T * m_data = nullptr;
        size_t m_size = 0;
    };
    
    // Vector over a fixed amount of items
    template <typename T, size_t N>
    class FixedVector {
    public:
        constexpr FixedVector() = default;
        
        constexpr T * data() { return m_data; }
        constexpr const T * data() const { return m_data; }
        constexpr size_t size() const { return m_size; }
        constexpr size_t capacity() const { return N; }
        
        constexpr bool empty() const { return m_size == 0; }

        constexpr void pop() {
            if (!m_data || m_size == 0) kstd::panic("access on an empty fixed vector");
            m_data[--m_size].~T();
        }

        constexpr void push(T && v) {
            if (!m_data || m_size == 0) kstd::panic("access on an empty fixed vector");
            if (m_size >= N) panic("out of bounds");
            m_data[m_size++] = v;
        }

        constexpr const Span<T> span() const { return Span<T>(m_data, N); }
        constexpr Span<T> span() { return Span<T>(m_data, N); }

        constexpr const T & operator[](size_t i) const {
            if (!m_data || m_size == 0) kstd::panic("access on an empty fixed vector");
            if (i >= N) panic("out of bounds");
            return m_data[i];
        }

        constexpr T & operator[](size_t i) {
            if (!m_data || m_size == 0) kstd::panic("access on an empty fixed vector");
            if (i >= N) panic("out of bounds");
            return m_data[i];
        }

        constexpr T * begin() { return m_data; }
        constexpr const T * begin() const { return m_data; }
        constexpr T * end() { return m_data + m_size; }
        constexpr const T * end() const { return m_data + m_size; }
        
        constexpr operator Span<T>() { return Span<T>(m_data, N); }
        constexpr operator const Span<T>() const { return Span<T>(m_data, N); }
    private:
        T m_data[N] = { T() };
        size_t m_size = 0;
    };
}

#endif
