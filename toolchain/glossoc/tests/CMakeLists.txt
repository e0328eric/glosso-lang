cmake_minimum_required(VERSION 3.19)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(glossoc_test mainTest.cc
        ../Span.cc ../Error.cc ../FileIO.cc ../Lexer.cc
        ../Token.cc ../Utf8.cc)

target_compile_options(glossoc_test PRIVATE ${COMPILE_OPTIONS})

target_include_directories(glossoc_test PUBLIC ${CMAKE_SOURCE_DIR}/core)
target_include_directories(glossoc_test PUBLIC ${CMAKE_SOURCE_DIR}/toolchain/glossoc)
target_link_libraries(glossoc_test PRIVATE gtest_main glossoCore)

add_test(NAME glossocTest COMMAND glossoc_test)
