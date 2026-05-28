/* -===============================================================================
    Include
   ===============================================================================- */

#include <std.hpp>
#include <arch/arch.hpp>
#include <core/paging.hpp>
#include <core/heap.hpp>
#include <kernel/linker.hpp>

/* -===============================================================================
    Internals
   ===============================================================================- */

arch::Context main_context, set_context;

size_t v = 0;

void test() {
    kstd::log("in the set context");
}

/* -===============================================================================
    Entry point
   ===============================================================================- */

void init_arch() {
    // Interrupts initialization
    kstd::set_info("interrupts", '2');
    arch::interrupt::init();
    
    // Paging initialization
    kstd::set_info("paging", '3');
    uint64_t mem_base = 0x80000000ULL;
    uint64_t mem_size = 0x8000000ULL;
    arch::paging::init(mem_base, mem_base + mem_size);
    
    kstd::set_info("heap", '6');
    arch::heap::init();
    
    kstd::set_info("vmem", '4');
    arch::vmem::init();
    arch::vmem::dump();
    
    // Virtual memory initialization
    //kstd::set_info("vmem", '6');
    //arch::vmem::init();
}

void init_misc() {
    main_context.pc = kstd::raw_address(init_misc);
    main_context.sp = kernel::linker::stack().start() + 2048;
    main_context.mode = 1;
    set_context.pc = kstd::raw_address(test);
    set_context.sp = kernel::linker::stack().start() + 2048;
    set_context.mode = 1;
    
    arch::interrupt::set_context_handler(&main_context);
    arch::interrupt::set_timer(5000000, &set_context);
    
    kstd::log("waiting for timer to fire, %zu", v++);
    while (1);
}

extern "C" void kernel_main(size_t hart_id, const void * dtb_ptr) {
    // Kernel initialization
    kstd::set_info(nullptr, '1');
    kstd::log("pawsOS started (using hart #%zu and DTB at %p)", hart_id, dtb_ptr);
    kstd::log("Kernel is %zx - %zx", kernel::linker::kernel().start(), kernel::linker::kernel().end());
    
    init_arch();
    
    kstd::set_info(nullptr, '1');
    //init_misc();
    
    while (1) asm volatile("wfi");
}

// -=- -=- Unused shit zone!!!!!!!!!!! -=- -=-

// rv::vmem::VirtualContext ctx;
// [[maybe_unused]]
// auto ptr = ctx.set(rv::vmem::VirtualAddress(0x00000000ULL), kernel::paging::page_to_region(page), rv::vmem::KERNEL_PAGE_FLAGS);
// rv::vmem::set(&ctx);
// rv::vmem::vmem_enable();

// * reinterpret_cast<volatile uint64_t *>(0x00000000ULL) = 5;
// kstd::log("test %zx", * reinterpret_cast<volatile uint64_t *>(0x00000000ULL));
