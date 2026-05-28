#pragma once
#ifndef ARCH_RV_ATOMIC_H
#define ARCH_RV_ATOMIC_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base.hpp>
#include <arch/rv/sbi.hpp>

/* -===============================================================================
    Platform functions
   ===============================================================================- */

namespace rv {
    // Synchronizes the memory barrier
    inline void sync() {
        __sync_synchronize();
    }
}

/* -===============================================================================
    Atomic
   ===============================================================================- */

namespace rv {
    // Follows strong ordering
    template <typename T>
    class Atomic {
    public:
        Atomic() = default;
        constexpr explicit Atomic(T v) : m_value(v) {}
    
        NO_COPY(Atomic);
    
        static_assert(
            sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
            "atomic types must be 1, 2, 4, or 8 bytes"
        );
        static_assert(
            kstd::type::is_trivially_copyable_v<T>, "atomic types must be trivially copyable"
        );
        
        // Gets the value within the handle
        inline T load() const { return __atomic_load_n(&m_value, __ATOMIC_SEQ_CST); }
        
        // Sets the value within the handle
        inline void store(T target) { __atomic_store_n(&m_value, target, __ATOMIC_SEQ_CST); }
        
        // Sets the value within the handle and returns the previous
        inline T exchange(T target) { return __atomic_exchange_n(&m_value, target, __ATOMIC_SEQ_CST); }
        
        // If the handle == [expected], set it to [target], if not, set [expected] to the current value
        inline bool compare_exchange(T & expected, T target) {
            return __atomic_compare_exchange_n(
                &m_value, &expected, target, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST
            );
        }
        
        inline T fetch_add(T v) {
            static_assert(sizeof(T) >= 4, "addition on sub-word atomics has overflow behavior");
            return __atomic_fetch_add(&m_value, v, __ATOMIC_SEQ_CST);
        }
        
        inline T fetch_sub(T v) {
            static_assert(sizeof(T) >= 4, "subtraction on sub-word atomics has overflow behavior");
            return __atomic_fetch_sub(&m_value, v, __ATOMIC_SEQ_CST);
        }
        
        inline T fetch_or(T v) { return __atomic_fetch_or (&m_value, v, __ATOMIC_SEQ_CST); }
        inline T fetch_and(T v) { return __atomic_fetch_and(&m_value, v, __ATOMIC_SEQ_CST); }
        inline T fetch_xor(T v) { return __atomic_fetch_xor(&m_value, v, __ATOMIC_SEQ_CST); }
        
        constexpr T operator++() { return fetch_add(1) + 1; }
        constexpr T operator++(int) { return fetch_add(1); }
        constexpr T operator--() { return fetch_sub(1) - 1; }
        constexpr T operator--(int) { return fetch_sub(1); }
        
        constexpr T operator=(T val) { store(val); return val; }
        constexpr explicit operator T() const { return load(); }
    private:
        alignas(sizeof(T)) T m_value = T();
    };
}

#endif
