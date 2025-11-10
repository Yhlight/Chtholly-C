# Building Chtholly

This document provides instructions for building the Chtholly compiler and specifying a custom C++ compiler.

## Prerequisites

- CMake 3.10 or higher
- A C++17 compliant compiler (e.g., GCC, Clang, MSVC)

## Building the Project

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/chtholly.git
    cd chtholly
    ```

2.  **Run the build script:**
    ```bash
    python3 scripts/build.py build
    ```
    This will create a `build` directory, run CMake to generate the build files, and compile the project. The `chtholly` executable will be located in `build/src`.

## Specifying a Custom C++ Compiler

By default, Chtholly uses the C++ compiler detected by CMake. You can specify a different compiler using the `--cxx` command-line argument.

### Usage

```bash
./build/src/chtholly --cxx <path-to-compiler> <your-script.cns>
```

### Example

To use Clang++ instead of the default G++, you would run:

```bash
./build/src/chtholly --cxx /usr/bin/clang++ my_program.cns
```

This is useful for testing with different compilers or when the desired compiler is not in the system's default search paths.
