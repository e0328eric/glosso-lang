cmake_minimum_required(VERSION 3.21)

project(glosso_lang)

set(CMAKE_CXX_STANDARD 20)
set(COMPILE_OPTIONS -O3 -Wall -Wextra -Wpedantic -Wconversion -Werror=return-type)

add_subdirectory(lib)
add_subdirectory(core)
add_subdirectory(toolchain/glossovm)
add_subdirectory(toolchain/olfactory)

install(TARGETS glossovm DESTINATION ~/.local/bin)
install(TARGETS olfactory DESTINATION ~/.local/bin)

#if (CMAKE_BUILD_TYPE STREQUAL Debug)
#    include(FetchContent)
#    FetchContent_Declare(
#            googletest
#            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#    )
#    if (WIN32)
#        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#    endif (WIN32)
#    FetchContent_MakeAvailable(googletest)
#
#    add_subdirectory(toolchain/glossoc/tests)
#endif ()
