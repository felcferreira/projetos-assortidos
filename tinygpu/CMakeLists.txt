cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(tinygpu LANGUAGES CXX VERSION 0.1.0)

file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(include/)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED raylib)

target_link_libraries(${PROJECT_NAME} PUBLIC ${RAYLIB_LIBRARIES} m)
target_include_directories(${PROJECT_NAME} PUBLIC ${RAYLIB_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PUBLIC ${RAYLIB_CFLAGS_OTHERS})

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -O0)