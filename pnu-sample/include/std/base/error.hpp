#pragma once
#ifndef CORE_STD_BASE_ERROR_H
#define CORE_STD_BASE_ERROR_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base/core.hpp>

/* -===============================================================================
    Status code
   ===============================================================================- */

namespace kstd {
    // Standard statuses
    enum Status {
        STATUS_OK = 0, 
        STATUS_OTHER = 1, 
        STATUS_INVALID_UNWRAP, 
        STATUS_OUT_OF_BOUNDS, 
        STATUS_OUT_OF_MEMORY, 
        STATUS_INVALID_ADDRESS, 
        STATUS_UNALIGNED_ACCESS, 
        STATUS_UNAUTHORIZED_ACCESS, 
    };
    
    // Converts the standard statuses
    constexpr const char * status_str(Status v) {
        switch (v) {
            case STATUS_OK:
                return "ok";
            case STATUS_INVALID_UNWRAP:
                return "invalid unwrap";
            case STATUS_OUT_OF_BOUNDS:
                return "out of bounds";
            case STATUS_OUT_OF_MEMORY:
                return "out of memory";
            case STATUS_INVALID_ADDRESS:
                return "invalid address";
            case STATUS_UNALIGNED_ACCESS:
                return "unaligned access";
            case STATUS_UNAUTHORIZED_ACCESS:
                return "unauthorized access";
            default: break;
        }
        return "other";
    }
}

/* -===============================================================================
    Classes
   ===============================================================================- */

namespace kstd {
    // Gives an error if a class is not null 
    template <typename OkType, typename ErrorType>
    class [[nodiscard("cannot discard a result")]] Result {
    public:
        // NOTE: this is just to make it so it never gets used as a return value
        NO_COPY(Result); NO_MOVE(Result);
        
        // Constructs an ok result
        constexpr Result(OkType v) : m_ok(v) {}
        
        // Constructs an error result
        constexpr Result(ErrorType v) : m_error(v) {}
        
        constexpr bool ok() const { return m_error == 0; }
        constexpr bool error() const { return m_error; }
        
        constexpr OkType & unwrap();
        constexpr const OkType & unwrap() const;
        constexpr ErrorType & unwrap_error() { return m_error; }
        constexpr const ErrorType & unwrap_error() const { return m_error; }
    private:
        OkType m_ok;
        ErrorType m_error = static_cast<ErrorType>(0);
    };
    
    // Gives an error if a class is not null (reference version)
    template <typename OkType, typename ErrorType>
    class [[nodiscard("cannot discard a result")]] Result<OkType &, ErrorType> {
    public:
        // NOTE: this is just to make it so it never gets used as a return value
        NO_COPY(Result); NO_MOVE(Result);
        
        // Constructs an ok result
        constexpr Result(OkType & v) : m_ok(&v) {}
        
        // Constructs an error result
        constexpr Result(ErrorType v) : m_error(v) {}
        
        constexpr bool ok() const { return m_error == 0; }
        constexpr bool error() const { return m_error; }
        
        constexpr OkType & unwrap();
        constexpr const OkType & unwrap() const;
        constexpr ErrorType & unwrap_error() { return m_error; }
        constexpr const ErrorType & unwrap_error() const { return m_error; }
    private:
        OkType * m_ok;
        ErrorType m_error = static_cast<ErrorType>(0);
    };
    
    // Gives an error if a class is not null (void version)
    template <typename ErrorType>
    class [[nodiscard("cannot discard a result")]] Result<void, ErrorType> {
    public:
        // NOTE: this is just to make it so it never gets used as a return value
        NO_COPY(Result); NO_MOVE(Result);
        
        // Constructs an ok result
        constexpr Result() = default;
        
        // Constructs an error result
        constexpr Result(ErrorType v) : m_error(v) {}
        
        inline bool ok() const { return m_error == 0; }
        inline bool error() const { return m_error; }
        
        constexpr ErrorType & unwrap_error() { return m_error; }
        constexpr const ErrorType & unwrap_error() const { return m_error; }
    private:
        ErrorType m_error = static_cast<ErrorType>(0);
    };
}

/* -===============================================================================
    Functions
   ===============================================================================- */

namespace kstd {    
    // Sets information about the logs
    extern void set_info(const char * module, char color);

    // Log function
    extern void log(const char * fmt, ...);
    
    // Log function
    extern void logv(const char * fmt, va_list args);
    
    // Panic function
    extern void panic(const char * fmt, ...);
    
    // Panic function with a particular standard error
    extern void panic(Status v);
}

/* -===============================================================================
    Class functions
   ===============================================================================- */

namespace kstd {
    template <typename OkType, typename ErrorType>
    constexpr OkType & Result<OkType, ErrorType>::unwrap() {
        if (error()) panic("invalid unwrap");
        return m_ok;
    }
    
    template <typename OkType, typename ErrorType>
    constexpr const OkType & Result<OkType, ErrorType>::unwrap() const {
        if (error()) panic("invalid unwrap");
        return m_ok;
    }
    
    template <typename OkType, typename ErrorType>
    constexpr OkType & Result<OkType &, ErrorType>::unwrap() {
        if (error() || !m_ok) panic("invalid unwrap");
        return * m_ok;
    }
    
    template <typename OkType, typename ErrorType>
    constexpr const OkType & Result<OkType &, ErrorType>::unwrap() const {
        if (error() || !m_ok) panic("invalid unwrap");
        return * m_ok;
    }
}

#endif
