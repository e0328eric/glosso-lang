# glosso-lang

Experimental language development inspired by [Tsoding's bm](https://github.com/tsoding/bm)

# Features

- [x] Implicit type conversion for arithmetic
- [x] Heap allocation (manually managed)
- [x] Implement Command IO
- [x] Turing Complete (proved by implementing a basic brainfuck interpreter with fixed memory)
- [x] Minimal Preprocessor Support

# For the beta version

- [ ] Dynamically sized stack
- [ ] Array in the stack
- [ ] glosso language can be compiled into C code properly

# For the stable version

- [ ] Internal garbage collection
- [ ] Implement a new language whose target is glasm

# Build

Currently, I tested this build script at macos and linux (especially, Arch Linux).

## using aedif

[aedif](https://github.com/e0328eric/aedif) is a build tool implemented by myself.

```console
$ aedif build
```

# Toolchains

## glossovm

A virtual machine that executes glosso binary (`.gsm` files)

## olfactory

Assembler for glosso binary. Uses `.glasm` file for input in general

## example usage

```console
$ aedif build
$ ./build/bin/olfactory ../examples/fib.glasm -o fib.gsm
$ ./build/bin/glossovm fib.gsm
```
