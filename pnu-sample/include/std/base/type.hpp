#pragma once
#ifndef CORE_STD_BASE_TYPE_H
#define CORE_STD_BASE_TYPE_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base/core.hpp>

/* -===============================================================================
    Type manipulation
   ===============================================================================- */

namespace kstd::type {
    // Removes a reference from a type
    template <typename T> struct remove_reference { using type = T; };
    
    // (implementation)
    template <typename T> struct remove_reference<T &> { using type = typename remove_reference<T>::type; };
    template <typename T> struct remove_reference<T &&> { using type = typename remove_reference<T>::type; };
    
    // Removes const from a type
    template <typename T>
    struct remove_const { using type = T; };
    
    // (implementation)
    template <typename T>
    struct remove_const<const T> { using type = T; };
    template <typename T>
    struct remove_const<const volatile T> { using type = volatile T; };
    
    // Removes volatile from a type
    template <typename T>
    struct remove_volatile { using type = T; };
    
    // (implementation)
    template <typename T>
    struct remove_volatile<volatile T> { using type = T; };
    template <typename T>
    struct remove_volatile<const volatile T> { using type = const T; };
    
    // Adds volatile to a type
    template <typename T>
    struct add_volatile { using type = volatile T; };
    
    // (implementation)
    template <typename T>
    struct add_volatile<volatile T> { using type = T; };
    template <typename T>
    struct add_volatile<const volatile T> { using type = const volatile T; };
}

/* -===============================================================================
    Type testing
   ===============================================================================- */
   
namespace kstd::type {
    // A simple integral constant
    template <typename T, T V>
    struct integral_constant {
        using type       = integral_constant;
        using value_type = T;
        
        static const constexpr T value = V;
    };

    using true_type  = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;
    
    // Checkes if a type is an lvalue
    template <typename T>
    struct is_lvalue_reference : false_type {};
    
    template <typename T>
    struct is_lvalue_reference<T &> : true_type {};
    
    // Checkes if a type is an rvalue
    template <typename T>
    struct is_rvalue_reference : false_type {};
    
    template <typename T>
    struct is_rvalue_reference<T &&> : true_type {};

    // Checkes if a type is the same as another
    template <typename T, typename U>
    struct is_same : false_type {};
    
    // (implementation)
    template <typename T>
    struct is_same<T, T> : true_type {};
}

/* -===============================================================================
    Type traits
   ===============================================================================- */

namespace kstd::type {
    // If the type is trivial/follows the standard layout
    template <typename T>
    using is_trivial = integral_constant<bool, __is_standard_layout(T)>;
    
    // If the type is trivially destructible
    template <typename T>
    using is_trivially_copyable = integral_constant<bool, __is_trivially_copyable(T)>;
    
    // If the type is trivially destructible
    template <typename T>
    using is_trivially_destructible = integral_constant<bool, __is_trivially_destructible(T)>;
}

/* -===============================================================================
    Aliasing
   ===============================================================================- */

namespace kstd::type {
    // Removes a reference from a type
    template <typename T>
    using remove_reference_t = typename remove_reference<T>::type;
    
    // Removes const from a type
    template <typename T>
    using remove_const_t = typename remove_const<T>::type;
    
    // Removes volatile from a type
    template <typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

    // Checkes if a type is an lvalue
    template <typename T>
    const constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
    
    // Checkes if a type is an rvalue
    template <typename T>
    const constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
    
    // Checkes if a type is the same as another
    template <typename T, typename U>
    const constexpr bool is_same_v = is_same<T, U>::value;

    // If the type is trivial/follows the standard layout
    template <typename T>
    const constexpr bool is_trivial_v = is_trivial<T>::value;
    
    // If the type is trivially copyable
    template <typename T>
    const constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

    // If the type is trivially destructible
    template <typename T>
    const constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
}

/* -===============================================================================
    Functions
   ===============================================================================- */

namespace kstd {
    template <typename T>
    constexpr auto move(T && t) {
        return static_cast<type::remove_reference_t<T> &&>(t);
    }
    
    template <class T>
    constexpr T && forward(type::remove_reference_t<T> & t) {
        return static_cast<T &&>(t);
    }
    
    template <class T>
    constexpr T && forward(type::remove_reference_t<T> && t) {
        static_assert(!type::is_lvalue_reference_v<T>, "cannot forward an rvalue as an lvalue");
        return static_cast<T &&>(t);
    }

}

#endif
