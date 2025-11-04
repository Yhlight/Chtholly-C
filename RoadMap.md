# Chtholly Development Roadmap

## Phase 1: Foundational Infrastructure

- [x] **Project Setup**: Initialize the project structure, including `src`, `tests`, and `scripts` directories.
- [x] **Build System**: Implement a CMake build system and a Python build script.
- [x] **Lexer**: Implement a lexer to tokenize the Chtholly source code.
- [x] **Parser**: Implement a parser to build an Abstract Syntax Tree (AST). (Note: Expression parsing has been refatored to use a Pratt parser for better operator-precedence handling.)
- [x] **AST**: Define the data structures for the AST.
- [x] **Basic Transpiler**: Implement a basic transpiler that can generate C++ code from a simple AST.

## Phase 2: Core Language Features

- [x] **Variables**: Implement `let` and `mut` variable declarations.
- [x] **Data Types**: Implement basic data types (int, double, bool, string).
- [x] **Operators**: Implement basic arithmetic and logical operators.
- [x] **Control Flow**: Implement `if`, `else`, `while`, and `switch` statements.
- [x] **Functions**: Implement function definitions and calls.

## Phase 3: Advanced Language Features

- [x] **Structs**: Implement struct definitions, instantiation, and field access.
- [x] **Access Modifiers**: Implement `public` and `private` access control for struct fields.
- [x] **Ownership System**: Implement the ownership system with `&` and `&mut` references.
- [x] **Generics**: Implement generic functions and structs.
- [x] **Lambda Expressions and Function Types**: Implement lambda expressions and function types.
- [x] **Traits**: Implement traits for defining shared behavior and operator overloading.
- [x] **Modules**: Implement the module system with `import`.

## Phase 4: Standard Library

- [x] **iostream**: Implement the `iostream` module for input and output.
- [x] **filesystem**: Implement the `filesystem` module for file system operations.
- [x] **operator**: Implement the `operator` module for operator overloading.
- [~] **reflect**: Implement the `reflect` module for static reflection. (Note: Static field name reflection has been implemented.)
- [~] **meta**: Implement the `meta` module for metaprogramming. (Note: Type-checking functions like `is_int`, `is_struct`, etc., have been implemented.)

## Phase 5: C++ Compiler Refactoring and Optimization

- [ ] **Code Cleanup**: Improve code style, consistency, and documentation.
- [ ] **Performance Analysis**: Profile the compiler to identify and address performance bottlenecks.
- [ ] **Error Reporting**: Enhance the error reporting mechanism to provide clearer and more informative diagnostics.
- [ ] **AST Enhancements**: Refine the Abstract Syntax Tree for better representation and easier traversal.
- [ ] **Test Coverage**: Increase test coverage to ensure compiler stability and correctness.

**Note on Self-Hosting**: The plan to bootstrap the compiler has been removed. As an MIT-licensed open-source community project, Chtholly will focus its efforts on developing and maintaining a single, high-quality C++ compiler rather than dividing resources to maintain two separate versions.
