#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"

TEST(FunctionTest, SimpleFunction) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(func add(int a, int b, ) -> int (block (return (+ a b))))");
}

TEST(FunctionTest, FunctionCall) {
    std::string source = "add(1, 2);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(; (call add 1 2))");
}
