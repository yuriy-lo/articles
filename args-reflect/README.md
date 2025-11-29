# args_reflect — minimal C++ CLI app

This repository contains a small C++ command-line application that demonstrates
an experimental technique: combining a runtime argument parser with compile-time
C++26 reflection to dispatch and convert function parameters automatically.

Example usage (command line parameters parsing and dispatching is done
by ```argparse::run``` function):
```c++
#include "argparse.h"

int sum(int x, int y);
int sqr(int v);

int main(int argc, char *argv[]) {
    return argparse::run<sum, sqr>(argc, argv);
}
```
```bash
./build/args_reflect sum --x 1 --y 2
```

Key points
- Built for and tested with a clang build that supports P2996 reflection.
- Uses an internal `parse_cmd` that parses `program command --name value` and
  `--name=value` forms into a `command` string and a map of parameter name -> value.
- `parse_cmd` only inserts a parameter when a value is actually provided.
  Standalone options like `--flag` are skipped by default (not inserted with an
  empty value).
- Conversions use `std::string_view` and `std::from_chars` where appropriate
  for zero-copy parsing (`convert_string<T>(std::string_view)`).
- The project contains `argparse::get_parameters_values<function>` helper that uses
  P2996 reflection to build a `std::tuple` of converted parameter values for a reflected
  function. The `run<...Functions>` template picks a function by its reflected
  identifier and calls it with typed arguments when parsing succeeds.
- Program prints no error messages. If program failed to parse parameters
  then it returns `1`.

Requirements
- Linux (tested on Ubuntu 24.04)
- CMake >= 3.16
- Ninja (recommended)
- A clang/clang++ compiler built from the p2996 branch (see [Building clang-p2996](#building-clang-p2996))

## Building and running

The project compiles in C++26 mode and enables experimental reflection. The
provided `toolchain-custom-llvm.cmake` file helps point CMake to a locally
built clang/clang++ that supports P2996.

Example build (set `LLVM_INSTALL_PREFIX` to your clang install):

```bash
cmake -S . -B build \
    -G Ninja \
    -DLLVM_INSTALL_PREFIX="/path/to/your/llvm-install" \
    -DCMAKE_TOOLCHAIN_FILE="toolchain-custom-llvm.cmake" \
    -DCMAKE_INSTALL_PREFIX=install \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build --config Debug
```

After build, run the executable and pass a command name plus parameters:

```bash
./build/args_reflect sum --x 1 --y 2
./build/args_reflect sqr --v 2
./build/args_reflect cat --prefix pre --root condition --suffix al
```

## Building clang-p2996

To build the `clang-p2996` compiler yourself (so you can point
this project at your locally-built `clang`/`clang++`), here are the exact
commands used to build LLVM/Clang on Ubuntu with Ninja:

```bash
git clone --depth 1 \
  --single-branch \
  --branch p2996 \
  https://github.com/bloomberg/clang-p2996 \
  llvm-p2996
mkdir llvm-build
mkdir llvm-install
cmake -G Ninja \
  -DLLVM_ENABLE_PROJECTS="clang" \
  -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi" \
  -DLIBCXXABI_USE_LLVM_UNWINDER=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=llvm-install \
  -S llvm-p2996/llvm \
  -B llvm-build
cmake --build llvm-build
cmake --build llvm-build --target install
```

Notes:
- Building LLVM/Clang can take a long time and requires significant RAM/CPU.
- Linux was chosen because libc++ from LLVM project is needed.
