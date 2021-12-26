if aedif.isclean then
else
LANGUAGE = "c"
COMPILER = "cc"
STD = 123
OPT_LEVEL = "s"
WARNINGS = { "all" }
aedif.compile("linenoise", "./lib/linenoise/linenoise.c", nil, nil, nil, "static")

--------------------------------------------------------------------------

LANGUAGE = "c++"
if aedif.ostype == "macos" then
    COMPILER = "g++-11"
elseif aedif.ostype == "linux" then
    COMPILER = "g++"
end
STD = 20
OPT_LEVEL = 3
WARNINGS = { "all", "extra", "pedantic" }
ERRORS = { "return-type" }

-- Core Configurations
core_target = "glossocore"
core_srcs = {
    "./core/Instruction.cc",
    "./core/Metadata.cc",
    "./core/SizeInt.cc",
    "./core/Value.cc",
}

aedif.compile(core_target, core_srcs, nil, nil, nil, "static")

-- Olfactory Configurations
olfactory_target = "olfactory"
olfactory_srcs = {
    "./toolchain/olfactory/Error.cc",
    "./toolchain/olfactory/FileIO.cc",
    "./toolchain/olfactory/Preprocessor.cc",
    "./toolchain/olfactory/Compiler.cc",
    "./toolchain/olfactory/main.cc",
}
olfactory_libs = "glossocore"
olfactory_lib_dirs = "build/lib"
olfactory_includes = "core"

aedif.compile(olfactory_target, olfactory_srcs, olfactory_libs,
              olfactory_lib_dirs, olfactory_includes, nil)

-- Glossovm Configurations
glossovm_target = "glossovm"
glossovm_srcs = {
    "./toolchain/glossovm/Error.cc",
    "./toolchain/glossovm/FileIO.cc",
    "./toolchain/glossovm/Vm.cc",
    "./toolchain/glossovm/Debugger.cc",
    "./toolchain/glossovm/main.cc",
}
glossovm_libs = {
    "linenoise",
    "glossocore",
}
glossovm_lib_dirs = "build/lib"
glossovm_includes = {
    "core",
    "lib/linenoise"
}

aedif.compile(glossovm_target, glossovm_srcs, glossovm_libs,
              glossovm_lib_dirs, glossovm_includes, nil)
end
