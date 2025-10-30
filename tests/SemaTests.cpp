#include <gtest/gtest.h>
#include "Sema.h"
#include "Parser.h"
#include "Lexer.h"

using namespace chtholly;

TEST(SemaTest, SimpleVariableDeclaration) {
    std::string source = "let a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Sema sema;
    EXPECT_NO_THROW(sema.analyze(*ast));
}

TEST(SemaTest, DuplicateVariableDeclaration) {
    std::string source = "let a = 10; let a = 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Sema sema;
    EXPECT_THROW(sema.analyze(*ast), std::runtime_error);
}

TEST(SemaTest, UndeclaredVariable) {
    std::string source = "let a = b;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Sema sema;
    EXPECT_THROW(sema.analyze(*ast), std::runtime_error);
}

TEST(SemaTest, TypeMismatchInBinaryExpression) {
    std::string source = "let a = 10 + \"hello\";";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Sema sema;
    EXPECT_THROW(sema.analyze(*ast), std::runtime_error);
}
