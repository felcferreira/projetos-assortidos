/* -===============================================================================
    Include
   ===============================================================================- */

#include <arch/rv/vmem.hpp>
#include <core/paging.hpp>
#include <std.hpp>

/* -===============================================================================
    Internals
   ===============================================================================- */

extern "C" void vmem_enable(rv::vmem::PageTable * root);
extern "C" void vmem_disable(rv::vmem::PageTable * root);

alignas(4096) rv::vmem::PageTable s_root_table;

static kstd::Result<rv::vmem::PageTable *, kstd::Status> s_get_or_alloc(rv::vmem::PageTable * table, size_t index) {
    auto * subtable = table->get_subtable(index);
    if (subtable) return subtable;
    
    auto res = arch::paging::alloc(1);
    if (res.error()) return res.unwrap_error();
    
    kstd::log("allocated for %zu", index);
    
    uintptr_t addr = arch::paging::page_to_address(res.unwrap());
    auto * ptr = kstd::raw_ptr<rv::vmem::PageTable>(addr);
    memset(ptr, 0, sizeof(rv::vmem::PageTable));
    
    table->set(index, addr, rv::vmem::PAGE_FLAG_V);
    return ptr;
}

/* -===============================================================================
    Virtual memory
   ===============================================================================- */

void rv::vmem::init() {
    memset(&s_root_table, 0, sizeof(PageTable));
    auto s = map(
        address_to_page(0x80000000ULL), address_to_page(0x8000000ULL), 
        kstd::align_up(arch::paging::reserved_memory().size(), PAGE_4K_SIZE) / PAGE_4K_SIZE, 
        KERNEL_PAGE_FLAGS
    );
    if (s != kstd::STATUS_OK)
        kstd::panic(kstd::status_str(s));
    vmem_enable(&s_root_table);
}

kstd::Status rv::vmem::map(size_t virt_page, size_t phys_page, size_t count, uint8_t flags) {
    kstd::log("requested %p[%zu] = %p", virt_page, count, phys_page);
    
    s_root_table.set(0, 0 * PAGE_1G_SIZE, KERNEL_PAGE_FLAGS);
    s_root_table.set(2, 2 * PAGE_1G_SIZE, KERNEL_PAGE_FLAGS);
    
    uintptr_t phys_addr = arch::paging::page_to_address(phys_page);
    
    for (size_t i = 0; i < count; ++i) {
        rv::vmem::VirtualAddress virt = arch::paging::page_to_address(virt_page) + i * PAGE_4K_SIZE;
        
        // root -> L2
        auto l2 = s_get_or_alloc(&s_root_table, virt.vpn2);
        if (l2.error()) return l2.unwrap_error();
        
        // L2 -> L1
        auto l1 = s_get_or_alloc(l2.unwrap(), virt.vpn1);
        if (l1.error()) return l1.unwrap_error();
        
        // L1 -> L0
        l1.unwrap()->set(virt.vpn0, phys_addr, flags | PAGE_FLAG_V | PAGE_FLAG_A | PAGE_FLAG_D);
        
        kstd::log("mapped [%zu][%zu][%zu] = %p", virt.vpn2, virt.vpn1, virt.vpn0, phys_addr);
        
        phys_addr += PAGE_4K_SIZE;
    }
    
    return kstd::STATUS_OK;
}

kstd::Status rv::vmem::unmap(size_t virt_page, size_t count) {
    
    return kstd::STATUS_OK;
}

void rv::vmem::dump() {
    s_root_table.get_subtable(2)->dump(0);
}

void rv::vmem::set(bool toggle) {
    if (toggle) vmem_enable(&s_root_table);
    else vmem_disable(&s_root_table);
}
