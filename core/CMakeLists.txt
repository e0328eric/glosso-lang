cmake_minimum_required(VERSION 3.19)

add_library(glossoCore Value.cc Instruction.cc Metadata.cc)
target_compile_options(glossoCore PRIVATE -O3 -Wall -Wextra -Wpedantic -Wconversion)
