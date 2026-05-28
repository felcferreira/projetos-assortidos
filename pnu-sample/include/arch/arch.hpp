#pragma once
#ifndef ARCH_H
#define ARCH_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <arch/rv/base.hpp>
#include <arch/rv/cpu.hpp>
#include <arch/rv/sbi.hpp>
#include <arch/rv/interrupt.hpp>
#include <arch/rv/atomic.hpp>
#include <arch/rv/vmem.hpp>

namespace arch {
    using namespace rv;
}

#endif
