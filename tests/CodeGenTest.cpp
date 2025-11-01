#include <gtest/gtest.h>
#include "CodeGen.h"
#include "Sema.h"
#include "Parser.h"
#include "Lexer.h"
#include <string>
#include <algorithm>

using namespace chtholly;

// Helper function to perform the full pipeline from source to generated C++
std::string generateCode(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Sema sema;
    sema.analyze(stmts);
    if (sema.hadError()) {
        return "Semantic errors occurred.";
    }
    CodeGen generator;
    return generator.generate(stmts);
}

// Helper to normalize whitespace for comparison
std::string normalize(const std::string& str) {
    std::string result;
    bool in_space = false;
    for (char c : str) {
        if (std::isspace(c)) {
            if (!in_space) {
                result += ' ';
                in_space = true;
            }
        } else {
            result += c;
            in_space = false;
        }
    }
    // Trim leading/trailing space
    size_t first = result.find_first_not_of(' ');
    if (std::string::npos == first) return "";
    size_t last = result.find_last_not_of(' ');
    return result.substr(first, (last - first + 1));
}


TEST(CodeGenTest, GeneratesVarDecl) {
    std::string source = "let a = 10;";
    std::string expected = "const auto a = 10;";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesIfStatement) {
    std::string source = "if (true) { let a = 1; } else { let b = 2; }";
    std::string expected = "if (true) { const auto a = 1; } else { const auto b = 2; }";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesForStatement) {
    std::string source = "for (mut i = 0; i < 10; i = i + 1) { 1; }";
    std::string expected = "for (auto i = 0; (i < 10); (i = (i + 1))) { 1; }";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesFunctionDeclaration) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    std::string expected = "auto add(int a, int b) -> int { return (a + b); }";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesMutVarDecl) {
    std::string source = "mut b = \"hello\";";
    std::string expected = "auto b = \"hello\";";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesExpressionStatement) {
    std::string source = "1 + 2;";
    std::string expected = "(1 + 2);";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesComplexExpression) {
    std::string source = "let a = (1 + 2) * 3;";
    std::string expected = "const auto a = ((1 + 2) * 3);";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesFunctionCall) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; } let c = add(1, 2);";
    std::string expected = "auto add(int a, int b) -> int { return (a + b); } const auto c = add(1, 2);";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesLambdaExpression) {
    std::string source = "let add = [](a: int, b: int) -> int { return a + b; };";
    std::string expected = "const auto add = [](int a, int b) -> int { return (a + b); };";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesSwitchStatement) {
    std::string source = "let x = 1; switch (x) { case 1: { break; } case 2: { fallthrough; } default: { break; } }";
    std::string expected = "const auto x = 1; switch (x) { case 1: { break; } case 2: { [[fallthrough]]; } default: { break; } }";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}

TEST(CodeGenTest, GeneratesStruct) {
    std::string source = "struct Point { x: int; y: int; } let p = Point { x: 1, y: 2 }; let a = p.x;";
    std::string expected = "struct Point { int x; int y; }; const auto p = Point{ .x = 1, .y = 2 }; const auto a = p.x;";
    EXPECT_EQ(normalize(generateCode(source)), normalize(expected));
}
