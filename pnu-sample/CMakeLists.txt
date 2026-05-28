# -========================================================
#  CMake
# ========================================================-

cmake_minimum_required(VERSION 3.21+)

# -========================================================
#  Barebones requirement
# ========================================================-

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)

set(CMAKE_C_COMPILER_TARGET riscv64-unknown-elf)
set(CMAKE_CXX_COMPILER_TARGET riscv64-unknown-elf)
set(CMAKE_ASM_COMPILER_TARGET riscv64-unknown-elf)

set(CMAKE_LINKER ld.lld)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# -========================================================
#  Project
# ========================================================-

project(kernel LANGUAGES C CXX ASM VERSION 0.0.1)

# -========================================================
#  C/C++ setup
# ========================================================-

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -========================================================
#  Settings
# ========================================================-


# -========================================================
#  Sources
# ========================================================-

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.s
    ${CMAKE_SOURCE_DIR}/third_party/src/*.c
    ${CMAKE_SOURCE_DIR}/third_party/src/*.cpp
)

add_executable(kernel.elf ${SOURCES})

# -========================================================
#  Disk image
# ========================================================-

# set(DISK_SRC_DIR ${CMAKE_SOURCE_DIR}/disk)
# set(DISK_DST_DIR ${CMAKE_BINARY_DIR}/disk)
# 
# add_custom_command(TARGET kernel.elf POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${DISK_DST_DIR}/boot
# 
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#             ${DISK_SRC_DIR}
#             ${DISK_DST_DIR}
# 
#     COMMAND ${CMAKE_COMMAND} -E copy
#             $<TARGET_FILE:kernel.elf>
#             ${DISK_DST_DIR}/boot/kernel.elf
# 
#     COMMENT "Copying disk directory and kernel.elf into build/disk/boot"
# )

# -========================================================
#  Dependencies
# ========================================================-



# -========================================================
#  Compiler
# ========================================================-

target_compile_options(kernel.elf PRIVATE
    -mcmodel=medany
    -march=rv64gc
    -mabi=lp64d
    
    -ffreestanding
    -fno-builtin
    -fno-stack-protector
    -fno-omit-frame-pointer
    -fno-optimize-sibling-calls
    -fno-pic
    -fno-pie
    -fno-rtti
    -fno-exceptions
    -fno-threadsafe-statics
    -nostdinc++
    
    -falign-functions=4
    -Wall
    -Wextra
    -O1
    -g
)

# -========================================================
#  Linker
# ========================================================-

target_link_options(kernel.elf PRIVATE
    --target=riscv64-unknown-elf
    -nostdlib
    -mno-relax
    -T ${CMAKE_SOURCE_DIR}/src/linker.ld
    -z max-page-size=4096
)

# -========================================================
#  Includes
# ========================================================-

target_include_directories(kernel.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/include
)
