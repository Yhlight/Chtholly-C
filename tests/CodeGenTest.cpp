#include "TestHelpers.h"
#include "gtest/gtest.h"
#include "CodeGen.h"
#include "Sema.h"
#include "Parser.h"
#include "Lexer.h"
#include <string>

using namespace chtholly;

const std::string PREAMBLE = "#include <string>\n#include <vector>\n#include <array>\n";

TEST(CodeGenTest, GeneratesVarDecl) {
    std::string source = "let a = 10;";
    std::string expected = PREAMBLE + "const int a = 10;";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesIfStatement) {
    std::string source = "if (true) { let a = 1; } else { let b = 2; }";
    std::string expected = PREAMBLE + "if (true) { const int a = 1; } else { const int b = 2; }";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesForStatement) {
    std::string source = "for (mut i = 0; i < 10; i = i + 1) { 1; }";
    std::string expected = PREAMBLE + "for (int i = 0; (i < 10); (i = (i + 1))) { 1; }";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesFunctionDeclaration) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    std::string expected = PREAMBLE + "auto add(int a, int b) -> int { return (a + b); }";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesMutVarDecl) {
    std::string source = "mut b = \"hello\";";
    std::string expected = PREAMBLE + "std::string b = \"hello\";";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesExpressionStatement) {
    std::string source = "1 + 2;";
    std::string expected = PREAMBLE + "(1 + 2);";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesComplexExpression) {
    std::string source = "let a = (1 + 2) * 3;";
    std::string expected = PREAMBLE + "const int a = ((1 + 2) * 3);";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesFunctionCall) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; } let c = add(1, 2);";
    std::string expected = PREAMBLE + "auto add(int a, int b) -> int { return (a + b); } const int c = add(1, 2);";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesLambdaExpression) {
    std::string source = "let add = [](a: int, b: int) -> int { return a + b; };";
    std::string expected = PREAMBLE + "const auto add = [](int a, int b) -> int { return (a + b); };";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesSwitchStatement) {
    std::string source = "let x = 1; switch (x) { case 1: { break; } case 2: { fallthrough; } default: { break; } }";
    std::string expected = PREAMBLE + "const int x = 1; switch (x) { case 1: { break; } case 2: { [[fallthrough]]; } default: { break; } }";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesStruct) {
    std::string source = "struct Point { x: int; y: int; } let p = Point { x: 1, y: 2 }; let a = p.x;";
    std::string expected = PREAMBLE + "struct Point { int x; int y; }; const auto p = Point{ .x = 1, .y = 2 }; const int a = p.x;";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesPrintStatement) {
    std::string source = "import iostream; print(1);";
    std::string expected = "#include <string>\n#include <vector>\n#include <array>\n#include <iostream>\nstd::cout << 1;";
    EXPECT_EQ(normalize(compile(source)), normalize(expected));
}
