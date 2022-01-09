cmake_minimum_required(VERSION 3.21)
project(linenoise)

add_library(linenoise linenoise/linenoise.c)
target_compile_options(linenoise PRIVATE -Os)
