# Building Chtholly Programs

This document provides instructions on how to build the Chtholly compiler and how to use it to compile your own `.cns` programs.

## Building the Chtholly Compiler

The Chtholly compiler uses CMake. You can build it using the provided Python build script.

```bash
# Build the compiler
python3 scripts/build.py build

# Build and run the tests
python3 scripts/build.py test
```

The `chtholly` executable will be located in the `build/src/` directory.

## Compiling Chtholly Programs

The `chtholly` compiler can be used to transpile `.cns` files into C++ and then invoke an external C++ compiler to create a native executable.

### Compiling a Single File

To compile a single file, simply pass the filename to the `chtholly` executable.

```bash
./build/src/chtholly my_program.cns
```

This will create an executable named `a.out` (on Unix-like systems) or `a.exe` (on Windows).

### Specifying the Output File

You can use the `-o` flag to specify the name of the output executable.

```bash
./build/src/chtholly my_program.cns -o my_awesome_program
```

This will create an executable named `my_awesome_program`.

### Specifying the C++ Compiler

Chtholly uses a C++ compiler (like g++ or clang++) to compile the transpiled code. The build system will detect a default compiler, but you can specify a different one using the `--cxx` flag.

```bash
./build/src/chtholly my_program.cns --cxx /path/to/my/custom/g++
```

This is useful if you have multiple C++ compilers installed or need to use a specific version.

### Compiling and Linking Multiple Files

You can compile and link multiple `.cns` files together into a single executable. Simply list all the source files.

```bash
./build/src/chtholly main.cns utils.cns -o my_multi_file_program
```

**Important:** When compiling multiple files, exactly one of those files must contain a `main` function, which will serve as the entry point for your program. The compiler will raise an error if zero or more than one `main` function is found.
