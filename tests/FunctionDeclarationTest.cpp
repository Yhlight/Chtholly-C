#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"

TEST(FunctionDeclarationTest, CorrectlyParsesFunctionDeclaration) {
    std::string source = "func add(x: int, y: int) -> int { }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    Chtholly::ASTPrinter printer;
    std::string result = printer.print(statements);

    std::string expected = "(func add(x: int, y: int) -> int { \n})";
    ASSERT_EQ(result, expected);
}

TEST(FunctionDeclarationTest, CorrectlyParsesFunctionWithNoParametersOrReturn) {
    std::string source = "func nop() { }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    Chtholly::ASTPrinter printer;
    std::string result = printer.print(statements);

    std::string expected = "(func nop() { \n})";
    ASSERT_EQ(result, expected);
}
