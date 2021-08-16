cmake_minimum_required(VERSION 3.19)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(glossoc
    main.cc Span.cc Error.cc FileIO.cc
    Lexer.cc Token.cc Utf8.cc)

target_compile_options(glossoc PRIVATE ${COMPILE_OPTIONS})

target_include_directories(glossoc PUBLIC ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(glossoc PRIVATE glossoCore)
