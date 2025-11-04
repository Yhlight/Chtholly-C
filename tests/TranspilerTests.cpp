#include <gtest/gtest.h>
#include "Transpiler.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <string>

using namespace chtholly;

std::string transpile(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Transpiler transpiler;
    return transpiler.transpile(statements);
}

TEST(TranspilerTest, VariableDeclaration) {
    std::string source = "let x = 10;";
    std::string expected = "const auto x = 10;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, MutableVariableDeclaration) {
    std::string source = "mut y = \"hello\";";
    std::string expected = "auto y = \"hello\";\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, ExpressionStatement) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "1 + 2 * 3;\n";
    EXPECT_EQ(transpile(source), expected);
}
