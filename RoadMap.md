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
  - [x] Basic module import system
  - [x] `iostream` module with `print` function
  - [x] `iostream` module with `input` function
  - [x] `filesystem` module with `fs_read` and `fs_write`
  - [x] `math` module with basic math functions
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
  - [x] `operator::binary`
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
- [x] Error Handling
  - [x] `result<T, E>` type
  - [x] `result::pass(value)` and `result::fail(value)`
  - [x] `is_pass()` and `is_fail()` methods (instance and static)
  - [x] `.unwarp()` and `.unwarp_err()` methods
- [x] Type Casting
  - [x] `type_cast<T>(expr)`
- [x] Lambda expressions
  - [x] Basic lambda syntax `[]() {}`
  - [x] Parameter and return type syntax
  - [x] Variable capture `[var]`
- [x] Function types
  - [x] `function(T, U) -> R` syntax for type annotations

## Phase 6: Traits and Constraints
- [x] Trait declaration parsing
- [x] Struct `impl` clause parsing
- [x] Generic constraints parsing
- [x] Trait method transpilation
- [x] Generic constraints with traits
