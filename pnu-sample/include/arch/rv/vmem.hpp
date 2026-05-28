#pragma once
#ifndef ARCH_RV_VMEM_H
#define ARCH_RV_VMEM_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <arch/rv/base.hpp>
#include <core/paging.hpp>
#include <std.hpp>

/* -===============================================================================
    Constants
   ===============================================================================- */

namespace rv::vmem {
    const constexpr size_t PAGE_4K_SIZE = 0x1000;
    const constexpr size_t PAGE_2M_SIZE = 0x200000;
    const constexpr size_t PAGE_1G_SIZE = 0x40000000;
    const constexpr size_t PAGE_SIZE    = 4096;
    
    const constexpr size_t PPN_MASK = ((1ULL << 44) - 1);
    
    const constexpr uint64_t PAGE_FLAG_NONE = 0x000; 
    const constexpr uint64_t PAGE_FLAG_V    = 0x001; // Set if valid
    const constexpr uint64_t PAGE_FLAG_R    = 0x002; // Read permission
    const constexpr uint64_t PAGE_FLAG_W    = 0x004; // Write permission
    const constexpr uint64_t PAGE_FLAG_X    = 0x008; // Execute permission
    const constexpr uint64_t PAGE_FLAG_U    = 0x010; // Set if user-space
    const constexpr uint64_t PAGE_FLAG_G    = 0x020; // If used by more than one page
    const constexpr uint64_t PAGE_FLAG_A    = 0x040; // Set if accessed
    const constexpr uint64_t PAGE_FLAG_D    = 0x080; // Set if dirty
    
    const constexpr uint64_t KERNEL_PAGE_FLAGS = PAGE_FLAG_V | PAGE_FLAG_R | PAGE_FLAG_W | PAGE_FLAG_X | PAGE_FLAG_A | PAGE_FLAG_D;
}

/* -===============================================================================
    Internals
   ===============================================================================- */

namespace rv::vmem {
    struct VirtualAddress {
        constexpr VirtualAddress() = default;
        constexpr VirtualAddress(uintptr_t address) : 
            vpn0((address >> 12) & 0x1FFULL), 
            vpn1((address >> 21) & 0x1FFULL), 
            vpn2((address >> 30) & 0x1FFULL)
        {}
        constexpr VirtualAddress(kstd::MemoryRegion region) :
            VirtualAddress(region.start()) {}
        
        constexpr VirtualAddress(uint64_t m_vpn0, uint64_t m_vpn1, uint64_t m_vpn2) :
            vpn0(m_vpn0), vpn1(m_vpn1), vpn2(m_vpn2) {}
        
        constexpr uintptr_t address() const {
            uint64_t addr = (vpn0 << 12) | (vpn1 << 21) | (vpn2 << 30);
            if (vpn2 & 0x100) {
                addr |= 0xFFFFFF8000000000ULL; // sign-extend bit 38
            }
            return addr;
        }
        
        uint64_t vpn0 = 0, vpn1 = 0, vpn2 = 0;
    };
    
    struct alignas(PAGE_SIZE) PageTable {
        constexpr PageTable() = default;
        
        constexpr void set(uint64_t index, uintptr_t ptr, uint64_t flags) {
            if (index >= 512) kstd::panic("out of bounds");
    
            entries[index] = (((ptr >> 12) & PPN_MASK) << 10) | (flags & 0x3FFULL);
        }
        
        constexpr bool empty() const {
            for (size_t i = 0; i < 512; ++i) {
                if (entries[i] != 0) return false;
            }
            return true;
        }
        
        constexpr PageTable * get_subtable(uint64_t index) {
            auto flags = get_flags(index);
    
            if (!(flags & PAGE_FLAG_V)) return nullptr;
        
            if (flags & (PAGE_FLAG_R | PAGE_FLAG_W | PAGE_FLAG_X))
                return nullptr;
        
            return kstd::raw_ptr<PageTable>(get_address(index));
        }
        
        constexpr uintptr_t get_address(uint64_t index) const {
            if (index >= 512) kstd::panic("out of bounds");
            
            return (((entries[index] >> 10) & PPN_MASK) << 12);
        }
        
        constexpr uint64_t get_flags(uint64_t index) const {
            if (index >= 512) kstd::panic("out of bounds");
            return entries[index] & 0x3FFULL;
        }
        
        constexpr void dump(size_t depth) {
            size_t start = 0;
            size_t last = get_flags(0);
            
            for (size_t i = 0; i < 512; ++i) {
                uint64_t flags = get_flags(i);
                if (last == flags && i != 511) continue;
                
                char perms[9] = "--------";
                if (last & PAGE_FLAG_V) perms[0] = 'v';
                if (last & PAGE_FLAG_R) perms[1] = 'r';
                if (last & PAGE_FLAG_W) perms[2] = 'w';
                if (last & PAGE_FLAG_X) perms[3] = 'x';
                if (last & PAGE_FLAG_U) perms[4] = 'u';
                if (last & PAGE_FLAG_D) perms[5] = 'd';
                if (last & PAGE_FLAG_A) perms[6] = 'a';
                if (last & PAGE_FLAG_G) perms[7] = 'g';
                
                kstd::log(
                    "L%zu: %+3zu .. %-3zu | %s | %p", 
                    depth, start, i, perms, get_address(start)
                );
                
                auto * table = get_subtable(start);
                if (table) table->dump(depth + 1);
                
                last  = flags;
                start = i + 1;
            }
        }
    
        uint64_t entries[512] = { 0 };
    };
}

/* -===============================================================================
    Virtual memory
   ===============================================================================- */

namespace rv::vmem {
    // Initializes the virtual memory
    void init();
    
    // Sets a virtual page to a physical page and returns it's pointer
    // NOTE: setting the kernel region is forbidden
    kstd::Status map(size_t virt_page, size_t phys_page, size_t count, uint8_t flags);
    
    // Unmaps a virtual page
    kstd::Status unmap(size_t virt_page, size_t count);
    
    // Enables or disables virtual memory
    void set(bool toggle);
    
    // Dumps the virtual memory map
    void dump();
    
    // Converts an address to a compatible page
    constexpr size_t address_to_page(uintptr_t addr) {
        return kstd::align_down(addr, PAGE_4K_SIZE) / PAGE_4K_SIZE;
    }
    
    // Converts a page to an address
    constexpr uintptr_t page_to_address(size_t page) {
        return kstd::align_down(page, PAGE_4K_SIZE) * PAGE_4K_SIZE;
    }
}

#endif
