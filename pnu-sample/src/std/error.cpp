/* -===============================================================================
    Include
   ===============================================================================- */

#include <std/base/core.hpp>
#include <std/base/error.hpp>
#include <arch/arch.hpp>
#include <printf.h>

/* -===============================================================================
    Internal
   ===============================================================================- */

static const char * s_module = nullptr;
static char s_color = '4';

/* -===============================================================================
    Error
   ===============================================================================- */

void kstd::set_info(const char * module, char color) {
    s_module = module;
    s_color = color;
}

void kstd::log(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logv(fmt, args);
    va_end(args);
}

void kstd::logv(const char * fmt, va_list args) {
    char str[256];
    
    size_t idx = vsnprintf(str, 256, fmt, args);
    str[idx] = '\0';
    
    rv::puts("\033[3");
    rv::putc(s_module ? s_color : '4');
    rv::puts("m[");
    rv::puts(s_module ? s_module : "kernel");
    rv::puts("]: ");
    rv::puts("\033[0m");
    rv::puts(str);
    rv::putc('\n');
}

void kstd::panic(const char * fmt, ...) {
    kstd::set_info("PANIC", '5');
    log(fmt);
    while (1);
}
