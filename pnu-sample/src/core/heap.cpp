/* -===============================================================================
    Include
   ===============================================================================- */

#include <core/heap.hpp>
#include <core/paging.hpp>
#include <std/base.hpp>

namespace ah = arch::heap;

/* -===============================================================================
    Internal
   ===============================================================================- */

alignas(4096) static ah::SlabPage s_first_page;

static ah::SlabHeaderPage s_first_header_page;

/* -===============================================================================
    Slab header
   ===============================================================================- */

kstd::Result<uintptr_t, kstd::Status> ah::SlabHeader::alloc(size_t count) {
    // Allocates a page for this slab if not already
    if (!m_page) {
        auto res = paging::alloc(1);
        if (res.error()) return res.unwrap_error();
        m_page = kstd::raw_ptr<SlabPage>(paging::page_to_address(res.unwrap()));
    }
    
    // Finds the first free entries for the count
    size_t idx = m_dirty.find_first_free(count);
    if (idx == SIZE_MAX) return kstd::STATUS_OUT_OF_MEMORY;
    
    // Sets the sequence
    m_dirty.set(idx, count, true);
    return kstd::raw_address(m_page->entries[idx]);
}

void ah::SlabHeader::free(uintptr_t ptr, size_t count) {
    if (!is_within(ptr)) return;
    
    // Frees the entry
    size_t idx = (kstd::raw_address(m_page) - ptr) / SLAB_ENTRY_SIZE;
    m_dirty.set(idx, count, false);
    
    // Clears out the header to reduce memory stress
    if (m_dirty.is_empty()) {
        paging::free(paging::address_to_page(kstd::raw_address(m_page)), 1);
    }
}
        
bool ah::SlabHeader::is_within(uintptr_t ptr) const {
    if (!m_page) return false;
    return kstd::MemoryRegion(kstd::raw_address(m_page), kstd::raw_address(m_page) + PAGE_SIZE).is_within_range(ptr);
}

/* -===============================================================================
    Slab header page
   ===============================================================================- */

kstd::Result<uintptr_t, kstd::Status> ah::SlabHeaderPage::alloc(size_t count) {
    for (auto & h : headers) {
        auto res = h.alloc(count);
        if (res.error()) continue;
        return res.unwrap();
    }
    return kstd::STATUS_OUT_OF_MEMORY;
}

void ah::SlabHeaderPage::free(uintptr_t ptr, size_t count) {
    for (auto & h : headers) {
        if (!h.is_within(ptr)) continue;
        h.free(ptr, count);
        return;
    }
    kstd::panic("%p is not within any avaliable slab", ptr);
}

/* -===============================================================================
    Functions
   ===============================================================================- */

kstd::Result<uintptr_t, kstd::Status> ah::alloc_raw(size_t size) {
    if (size >= PAGE_SIZE) {
        auto page = paging::alloc(kstd::align_up(size, PAGE_SIZE) / PAGE_SIZE);
        if (page.error()) return page.unwrap_error();
        return paging::page_to_address(page.unwrap());
    }
    
    auto res = s_first_header_page.alloc(kstd::align_up(size, SLAB_ENTRY_SIZE) / SLAB_ENTRY_SIZE);
    if (res.error()) return res.unwrap_error();
    return res.unwrap();
}

void ah::free_raw(uintptr_t ptr, size_t size) {
    if (ptr == 0 || size == 0) return;
    
    //if (!paging::usable_memory().is_within_range(ptr))
    //    kstd::panic("attempt to free a page out of range (%p <-> %p - %p)", ptr, paging::usable_memory().start(), paging::usable_memory().end());
    
    if (size >= PAGE_SIZE) {
        paging::free(paging::address_to_page(ptr));
        return;
    }
    
    s_first_header_page.free(ptr, size / SLAB_ENTRIES);
}

void ah::init() {
    s_first_header_page = SlabHeaderPage();
    s_first_header_page.headers[0] = SlabHeader(&s_first_page);
}
