# args_reflect — minimal C++ CLI app

This repository contains a minimal C++ command-line application and CMake configuration.
The app simply prints the program name and all arguments it receives.

Requirements
- Linux (tested on Ubintu 24.04)
- CMake >= 3.16
- A working build tool (Ninja is recommended)
- Your custom clang/clang++ binaries (the LLVM/Clang fork used for P2996, see
[Building clang-p2996](#building-clang-p2996)).

## Building a program

This project always enables the clang P2996 experimental reflection features
(`-freflection-latest`) and compiles in C++26 mode. It defines necessary
variables to use custom clang compiler in [toolchain-custom-llvm.cmake](./toolchain-custom-llvm.cmake)
file.

Run the following to build (set `LLVM_INSTALL_PREFIX` to install location of your custom clang build):

```bash
cmake -S . -B build \
    -G Ninja \
    -DLLVM_INSTALL_PREFIX="/path/to/your/llvm-install" \
    -DCMAKE_TOOLCHAIN_FILE="toolchain-custom-llvm.cmake" \
    -DCMAKE_INSTALL_PREFIX=install \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build --config Debug
```

## Running the program

After a successful build you can run:

```bash
./build/args_reflect hello world
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
