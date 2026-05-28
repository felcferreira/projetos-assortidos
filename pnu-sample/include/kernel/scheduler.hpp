#pragma once
#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <std/base.hpp>
#include <core/arch.hpp>

/* -===============================================================================
    Constants
   ===============================================================================- */

namespace kernel::scheduler {
    
}

/* -===============================================================================
    Thread
   ===============================================================================- */

namespace kernel::scheduler {
    class Process;
    
    // A lane of execution associated with a process
    class Thread {
    public:
        Thread(Process & proc) : m_proc(proc) {}
    private:
        arch::Context m_context;
        
        Process & m_proc = nullptr;
    };
}

/* -===============================================================================
    Process
   ===============================================================================- */

namespace kernel::scheduler {
    // Allocates 
    class Process {
    public:
        Process() = default;
        
        void advance();
    private:
        arch::Context m_context;
        
        Process * m_proc = nullptr;
    };
}

/* -===============================================================================
    Process manager
   ===============================================================================- */

namespace kernel::scheduler {
    class ProcessManager {
    public:
        
    };
}

#endif
