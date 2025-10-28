#include "../src/CodeGen.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include <cassert>
#include <string>
#include <iostream>

void test_simple_code_generation() {
    std::string source = "let a = 10;";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    assert(ir.find("define i32 @main()") != std::string::npos);
    assert(ir.find("%a = alloca i64") != std::string::npos);
    assert(ir.find("store i64 10, ptr %a") != std::string::npos);
    assert(ir.find("ret i32 0") != std::string::npos);
}

void test_binary_expression_code_generation() {
    std::string source = "let x = 5 + 5 * 2;";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    // Check for constant folded value
    assert(ir.find("%x = alloca i64") != std::string::npos);
    assert(ir.find("store i64 15, ptr %x") != std::string::npos);
}

int main() {
    test_simple_code_generation();
    test_binary_expression_code_generation();
    return 0;
}
