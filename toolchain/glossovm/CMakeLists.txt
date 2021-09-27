cmake_minimum_required(VERSION 3.19)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(glossovm main.cc Error.cc FileIO.cc Vm.cc Debugger.cc)

target_compile_options(glossovm PRIVATE ${COMPILE_OPTIONS})

target_include_directories(glossovm PUBLIC ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(glossovm glossoCore)
