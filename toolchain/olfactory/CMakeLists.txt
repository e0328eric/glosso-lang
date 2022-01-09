cmake_minimum_required(VERSION 3.21)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(olfactory main.cc Error.cc FileIO.cc Preprocessor.cc Compiler.cc)

target_compile_options(olfactory PRIVATE ${COMPILE_OPTIONS})

target_include_directories(olfactory PUBLIC ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(olfactory glossoCore)
