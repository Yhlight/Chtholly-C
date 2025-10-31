#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"

TEST(ParserTest, SimpleBinaryExpression) {
    std::string source = "1 + 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(; (+ 1 2))");
}

TEST(ParserTest, LetStatement) {
    std::string source = "let a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(let a = 10)");
}

TEST(ParserTest, LetStatementWithType) {
    std::string source = "let a: int = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(let a: int = 10)");
}

TEST(ParserTest, MutStatement) {
    std::string source = "mut a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(mut a = 10)");
}

TEST(ParserTest, BlockStatement) {
    std::string source = "{ let a = 1; a + 2; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(block (let a = 1)(; (+ a 2)))");
}
