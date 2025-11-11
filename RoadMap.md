# Chtholly Project Roadmap

This document outlines the development progress and future goals for the Chtholly programming language.

## Phase 1: Project Setup and Foundation

- [x] Create project directory structure (`src`, `tests`, `scripts`)
- [x] Establish build system with CMake and Python build script
- [x] Implement core data structures (Token, TypeInfo)

## Phase 2: Lexer

- [x] Implement the Lexer to tokenize the Chtholly source code.
- [x] Add support for all keywords, operators, and literals defined in `Chtholly.md`.
- [x] Write comprehensive tests for the Lexer.

## Phase 3: Parser

- [x] Implement the Parser to build an Abstract Syntax Tree (AST).
- [x] Define the AST node hierarchy in `src/AST.h`.
- [x] Implement parsing for all language constructs (variables, functions, control flow, etc.).
  - [x] `if` statements
  - [x] `while` loops
  - [x] `for` loops
  - [x] `switch` statements
- [x] Write comprehensive tests for the Parser.

## Phase 4: Transpiler

- [x] Implement the Transpiler to convert the AST to C++ code.
  - [x] Expression and basic statement transpilation
- [x] Implement visitors for all AST node types.
- [x] Add support for all language features, including generics, traits, and modules.
- [x] Write comprehensive tests for the Transpiler.

## Phase 5: Advanced Features

- [x] Generics
  - [x] Generic functions
  - [x] Generic structs

- [x] Implement the standard library (`iostream`, `filesystem`, etc.).
  - [x] Refactor `string` module to object-oriented style
  - [x] Refactor `array` module to object-oriented style
  - [x] Basic module import system
  - [x] `iostream` module with `print` function
  - [x] `iostream` module with `input` function
  - [x] `filesystem` module with `fs_read` and `fs_write`
  - [x] `math` module with functions and constants
  - [x] `string` module with `length`, `substr`, `find`, `split`, `join`, `is_empty`, `replace`, and `contains`
  - [x] `array` module with `length`, `push`, `pop`, `contains`, `reverse`, `is_empty`, `clear`, and `sort`
  - [x] `os` module with `exit` and `env`
  - [x] `random` module with `rand` and `randint`
  - [x] `time` module with `now`
- [x] Implement static reflection and metaprogramming features.
  - [x] Symbol table and basic type analysis engine
  - [x] `meta` module with `is_int`
  - [x] `meta` module with `is_uint`
  - [x] `meta` module with `is_double`
  - [x] `meta` module with `is_char`
  - [x] `meta` module with `is_bool`
  - [x] `meta` module with `is_string`
  - [x] `meta` module with `is_struct`
  - [x] `meta` module with `is_array`
  - [x] `meta` module with `is_let`
  - [x] `meta` module with `is_mut`
  - [x] `meta` module with `is_borrow`
  - [x] `meta` module with `is_borrow_mut`
  - [x] `meta` module with `is_move`
  - [x] `meta` module with `type_name`
  - [x] `reflect` module with `get_field_count`
  - [x] `reflect` module with `get_fields`
  - [x] `reflect` module with `get_field`
  - [x] `reflect` module with `get_method_count`
  - [x] `reflect` module with `get_methods`
  - [x] `reflect` module with `get_method`
  - [x] `reflect` module with `get_trait_count`
  - [x] `reflect` module with `get_traits`
  - [x] Refactor `reflect` module to use built-in structs
  - [x] `util` module with `string_cast` and `to_string` trait
  - [x] `util` module with `panic`
- [x] Implement operator overloading.
  - [x] `operator::add`
  - [x] `operator::sub`
  - [x] `operator::mul`
  - [x] `operator::div`
  - [x] `operator::mod`
  - [x] `operator::assign`
  - [x] `operator::not_equal`
  - [x] `operator::less`
  - [x] `operator::less_equal`
  - [x] `operator::greater`
  - [x] `operator::greater_equal`
  - [x] `operator::and`
  - [x] `operator::or`
  - [x] `operator::not`
  - [x] `operator::assign_add`
  - [x] `operator::assign_sub`
  - [x] `operator::assign_mul`
  - [x] `operator::assign_div`
  - [x] `operator::assign_mod`
  - [x] `operator::prefix_add`
  - [x] `operator::prefix_sub`
  - [x] `operator::postfix_add`
  - [x] `operator::postfix_sub`
  - [x] `operator::binary` (custom binary operators)
- [x] Arrays
  - [x] Dynamic arrays (`array[T]`)
  - [x] Fixed-size arrays (`array[T; N]`)
  - [x] Array literal parsing and transpilation
  - [x] Type inference for array literals
- [x] Enums
  - [x] Enum declaration parsing and transpilation
- [x] Null safety
  - [x] `option<T>` type
  - [x] `none` literal
  - [x] `.unwarp()` and `.unwarp_or()` methods
  - [x] Implement `option(value)` constructor
- [x] Error Handling
  - [x] `result<T, E>` type
  - [x] `result::pass(value)` and `result::fail(value)`
  - [x] Fix unsafe static `is_pass`/`is_fail` implementation
  - [x] `.unwarp()` and `.unwarp_err()` methods
- [x] Type Casting
  - [x] `type_cast<T>(expr)`
- [x] Lambda expressions
  - [x] Basic lambda syntax `[]() {}`
  - [x] Parameter and return type syntax
  - [x] Variable capture `[var]`
- [x] Function types
  - [x] `function(T, U) -> R` syntax for type annotations
- [x] Struct Initialization
  - [x] Named field initialization `MyStruct{ field: value }`
  - [x] Positional field initialization `MyStruct{ value1, value2 }`

## Phase 6: Traits and Constraints
- [x] Trait declaration parsing
- [x] Struct `impl` clause parsing
- [x] Generic constraints parsing
- [x] Trait method transpilation
- [x] Implement transpilation for generic constraints

## Phase 7: Architectural Issues and Long-Term Goals

- [x] Define and implement the program execution model (handling `main`'s return value)
- [x] **Improved Build and Compilation Process**: Enhance the compiler executable to support multi-file compilation, allow specifying the output file (`-o`), and let users provide a path to a custom C++ compiler (`--cxx`).

## Phase 8: Cross-Platform Support
- [x] Ensure compiler and generated code are compatible with both Windows and Unix-like systems.

## Phase 9: Compiler Architecture Refactoring (High Priority)

Based on a comprehensive review, the next major focus is to refactor the compiler's architecture to improve robustness, maintainability, and extensibility.

- [ ] **Introduce a Separate Semantic Analysis Phase (Resolver):**
  - Create a new compiler stage that runs after the Parser and before the Transpiler.
  - This stage will be responsible for:
    - [ ] Building a hierarchical symbol table.
    - [ ] Resolving all variable, function, and type identifiers.
    - [x] Performing comprehensive type checking for all expressions and statements.
      - [x] Variable declarations (`let`, `mut`)
      - [x] Assignment expressions (`=`)
      - [x] Unary and binary operators
      - [x] Function calls
      - [x] Struct field access
      - [x] Struct initializers
      - [x] Return statements
      - [x] `if`, `while`, and `for` condition statements
  - The goal is to decouple semantic analysis from the code generation process.

- [ ] **Refactor the Transpiler:**
  - Simplify the `Transpiler` by removing all type inference and semantic validation logic.
  - The `Transpiler` should assume a semantically correct and type-annotated AST.
  - Its sole responsibility will be to translate the AST into C++ code.

- [ ] **Enhance the Type System:**
  - Move away from using `std::string` to represent types within the compiler.
  - Implement a dedicated class hierarchy for the type system (e.g., `Type`, `IntType`, `StructType`) to enable more robust type comparisons and analysis.

- [ ] **Improve Parser Error Recovery:**
  - Enhance the `synchronize()` method in the `Parser` to provide more intelligent error recovery.
  - Implement finer-grained synchronization points (e.g., at the end of blocks `}`) to minimize discarded code and improve the accuracy of subsequent error messages.
