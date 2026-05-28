/* -===============================================================================
    Include
   ===============================================================================- */

#include <core/paging.hpp>
#include <kernel/linker.hpp>
#include <std/base.hpp>

/* -===============================================================================
    Internal
   ===============================================================================- */

static kstd::MemoryRegion s_total_mem;
static kstd::MemoryRegion s_avail_mem;
static kstd::MemoryRegion s_map_mem;
static kstd::BitmapSpan   s_map_bitmap;

static size_t s_start_page = 0;
static size_t s_end_page   = 0;

/* -===============================================================================
    Memory
   ===============================================================================- */

namespace ap = arch::paging;

kstd::Status ap::init(uintptr_t mem_start, uintptr_t mem_end) {
    // The total amount of memory
    s_total_mem = kstd::MemoryRegion(mem_start, mem_end);
    
    s_avail_mem = kstd::MemoryRegion(
        kstd::align_up(kernel::linker::kernel().end(), PAGE_SIZE), 
        kstd::align_down(mem_end, PAGE_SIZE)
    );
    
    // Amount of reserved 4KiB pages for the mappings
    size_t reserved = total_pages() / 8;
    s_map_mem.set_start(s_avail_mem.start());
    s_map_mem.set_size(kstd::align_up(reserved, PAGE_SIZE));
    
    s_start_page = s_avail_mem.start() / PAGE_SIZE;
    s_end_page   = s_avail_mem.end() / PAGE_SIZE;
    
    //s_start_page = 512;
    //s_end_page   = s_start_page + kstd::align_down(s_avail_mem.size(), PAGE_SIZE) / PAGE_SIZE;
    
    kstd::log("Total memory %zx - %zx", s_total_mem.start(), s_total_mem.end());
    kstd::log("Kernel memory %zx - %zx", kernel::linker::kernel().start(), kernel::linker::kernel().end());
    kstd::log("Mappings memory %zx - %zx (%zu pages, %zu bytes)", s_map_mem.start(), s_map_mem.end(), reserved, total() / 8);
    kstd::log("Usable memory %zx - %zx (%zu pages, %zu bytes)", s_avail_mem.start(), s_avail_mem.end(), total_pages(), total());
    kstd::log("Base pages %zu - %zu", s_start_page, s_end_page);
    
    s_map_bitmap = kstd::BitmapSpan(kstd::MemoryRegion(s_map_mem.start(), s_map_mem.end()));
    s_map_bitmap.set(0, s_map_bitmap.size_bits(), false);
    s_map_bitmap.set(0, reserved, true);
    
    return kstd::STATUS_OK;
}

size_t ap::total_pages() {
    return s_avail_mem.size() / PAGE_SIZE;
}

size_t ap::total() {
    return s_avail_mem.size();
}

kstd::Result<size_t, kstd::Status> ap::alloc(size_t count, size_t alignment) {
    if (count == 0) count = 1;

    size_t last_free = s_map_bitmap.find_first_free(count, alignment);
    if (last_free == SIZE_MAX) return kstd::STATUS_OUT_OF_MEMORY;
    
    s_map_bitmap.set(last_free, count, true);
    //kstd::log("allocated page index %zu + %zu", last_free, s_start_page);
    
    return last_free + s_start_page;
}

void ap::free(size_t page_index, size_t count) {
    if (count == 0) count = 1;
    //kstd::log("freed page index %zu + %zu", page_index, s_start_page);
    if (page_index < s_start_page || page_index >= s_end_page)
        kstd::panic("attempt to free a page out of range (%zx <-> %zx - %zx)", page_index, s_start_page, s_end_page);
    s_map_bitmap.set(page_index - s_start_page, count, false);
}

kstd::MemoryRegion ap::total_memory() { return s_total_mem; }
kstd::MemoryRegion ap::usable_memory() { return s_avail_mem; }
kstd::MemoryRegion ap::mapping_memory() { return s_map_mem; }

kstd::MemoryRegion ap::reserved_memory() {
    return kstd::MemoryRegion(kernel::linker::kernel().start(), s_map_mem.end());
}

uintptr_t ap::page_to_address(size_t index) {
    return s_avail_mem.start() + (index - s_start_page) * PAGE_SIZE;
}

size_t ap::address_to_page(uintptr_t ptr) {
    return (ptr - s_avail_mem.start()) / PAGE_SIZE + s_start_page;
}

void ap::dump() {
    s_map_bitmap.dump();
}
