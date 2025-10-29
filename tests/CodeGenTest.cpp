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

void test_function_code_generation() {
    std::string source = "func add(a, b) { return a + b; } let result = add(10, 20);";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    assert(ir.find("define i64 @add(i64 %a, i64 %b)") != std::string::npos);
    assert(ir.find("ret i64 %addtmp") != std::string::npos);
    assert(ir.find("%calltmp = call i64 @add(i64 10, i64 20)") != std::string::npos);
    assert(ir.find("%result = alloca i64") != std::string::npos);
    assert(ir.find("store i64 %calltmp, ptr %result") != std::string::npos);
}

void test_if_else_expression_code_generation() {
    std::string source = "if (10 > 5) { return 1; } else { return 0; }";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    assert(ir.find("br i1 %ifcond, label %then, label %else") != std::string::npos);
    assert(ir.find("then:") != std::string::npos);
    assert(ir.find("ret i64 1") != std::string::npos);
    assert(ir.find("else:") != std::string::npos);
    assert(ir.find("ret i64 0") != std::string::npos);
    assert(ir.find("ifcont:") != std::string::npos);
}

void test_while_statement_code_generation() {
    std::string source = "let a = 0; while (a < 10) { a = a + 1; }";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    assert(ir.find("loopcond:") != std::string::npos);
    assert(ir.find("loopbody:") != std::string::npos);
    assert(ir.find("loopexit:") != std::string::npos);
    assert(ir.find("br label %loopcond") != std::string::npos);
}

void test_logical_operators_code_generation() {
    std::string source = "let a = 10; let b = 20; if (a > 5 && b > 15) { return 1; } else { return 0; }";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    assert(ir.find("icmp sgt i64") != std::string::npos);
    assert(ir.find("and i1") != std::string::npos);
}

void test_nested_control_flow_code_generation() {
    std::string source = "let a = 0; while (a < 10) { if (a == 5) { return 1; } a = a + 1; } return 0;";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    assert(ir.find("loopcond:") != std::string::npos);
    assert(ir.find("loopbody:") != std::string::npos);
    assert(ir.find("loopexit:") != std::string::npos);
    assert(ir.find("ifcond:") != std::string::npos);
    assert(ir.find("then:") != std::string::npos);
    assert(ir.find("ret i64 1") != std::string::npos);
    assert(ir.find("ifcont:") != std::string::npos);
}

void test_double_code_generation() {
    std::string source = "let a = 10.5; let b = a + 1.0;";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    assert(ir.find("%a = alloca double") != std::string::npos);
    assert(ir.find("store double 1.050000e+01, ptr %a") != std::string::npos);
    assert(ir.find("%b = alloca double") != std::string::npos);
    assert(ir.find("fadd double") != std::string::npos);
}

int main() {
    test_simple_code_generation();
    test_binary_expression_code_generation();
    test_function_code_generation();
    test_if_else_expression_code_generation();
    test_while_statement_code_generation();
    test_logical_operators_code_generation();
    test_nested_control_flow_code_generation();
    test_double_code_generation();
    return 0;
}
