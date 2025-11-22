# toolchain-custom-llvm.cmake

# --- 1. Check for LLVM Installation Prefix ---
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES LLVM_INSTALL_PREFIX)
if(NOT LLVM_INSTALL_PREFIX)
    message(FATAL_ERROR 
        "LLVM_INSTALL_PREFIX must be set! Please provide it on the CMake command line: "
        "-DLLVM_INSTALL_PREFIX=/path/to/your/llvm-install"
    )
endif()

# Set the C and C++ compilers to your custom Clang
set(CMAKE_C_COMPILER   "${LLVM_INSTALL_PREFIX}/bin/clang" CACHE FILEPATH "C Compiler")
set(CMAKE_CXX_COMPILER "${LLVM_INSTALL_PREFIX}/bin/clang++" CACHE FILEPATH "C++ Compiler")

# Tell CMake this is a toolchain file (must be set before the first 'project' command)
set(CMAKE_TOOLCHAIN_FILE_EXTERNAL TRUE)
set(CMAKE_SYSTEM_NAME Linux) # Or Darwin for macOS, etc.

# --- 2. Configure libc++ and ABI linkage ---
# These flags instruct Clang to use libc++ (instead of libstdc++)
# and link against libc++ and libc++abi.

# C++ Flags (for compilation, sets the standard library)
set(CUSTOM_CXX_FLAGS "-stdlib=libc++" CACHE STRING "C++ Standard Library Flag")

# Linker Flags (for linking, links the library implementation)
# You may also need -lc++abi on some systems, but -stdlib=libc++ often handles the linkage.
set(CUSTOM_LINKER_FLAGS "-stdlib=libc++" CACHE STRING "C++ Linker Flag")

# Apply the flags globally
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_CXX_FLAGS}" CACHE STRING "C++ Flags")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CUSTOM_LINKER_FLAGS}" CACHE STRING "Executable Linker Flags")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${CUSTOM_LINKER_FLAGS}" CACHE STRING "Shared Library Linker Flags")

# --- 3. Linker Search Path (Optional but helpful) ---
# Set the path to your custom libc++ libraries explicitly.
set(LLVM_TARGET_TRIPLE "x86_64-unknown-linux-gnu" CACHE STRING "LLVM Target Triple")
set(CUSTOM_LIB_PATH "${LLVM_INSTALL_PREFIX}/lib/${LLVM_TARGET_TRIPLE}" CACHE PATH "Custom Library Path")
