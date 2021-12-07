# glosso-lang

Experimental language development inspired by [Tsoding's bm](https://github.com/tsoding/bm)

# Features

- [x] Implicit type conversion for arithmetic
- [x] Heap allocation (manually managed)
- [x] Implement Command IO
- [x] Turing Complete (proved by implementing a basic brainfuck interpreter with fixed memory)

# For the beta version

- [ ] Dynamically sized stack
- [ ] Array in the stack
- [ ] glosso language can be compiled into C code properly

# For the stable version

- [ ] Internal garbage collection
- [ ] Implement a new language whose target is glasm

# Build
Currently, only UNIX related os can build entire codes.

## using python script

This project can be build using python script.
It uses `clang` compiler in default.

```console
./cb b
```

# Toolchains

## glossovm

A virtual machine that executes glosso binary (`.gsm` files)

## olfactory

Assembler for glosso binary. Uses `.glasm` file for input in general

## example usage
```console
./cb b
./bin/olfactory ../examples/fib.glasm -o fib.gsm
./bin/glossovm fib.gsm
```
