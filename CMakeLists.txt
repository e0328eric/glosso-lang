cmake_minimum_required(VERSION 3.21)

if (APPLE)
    set(CMAKE_C_COMPILER "/usr/local/bin/gcc-11")
    set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-11")
endif ()

project(glosso_lang)

set(CMAKE_CXX_STANDARD 20)
set(COMPILE_OPTIONS -O3 -Wall -Wextra -Wpedantic -Wconversion -Werror=return-type)

enable_testing()

add_subdirectory(core)
add_subdirectory(lib/linenoise)
add_subdirectory(toolchain/glossovm)
add_subdirectory(toolchain/olfactory)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    if (WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif (WIN32)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(toolchain/glossoc/tests)
endif ()
