#include <gtest/gtest.h>
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/AST/ASTPrinter.h"

TEST(ParserTest, LetStatement) {
    std::string source = "let x = 10;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(let x = 10)\n");
}

TEST(ParserTest, LetStatementNoInitializer) {
    std::string source = "let y;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(let y)\n");
}

TEST(ParserTest, UnaryExpression) {
    std::string source = "let x = -10;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(let x = (- 10))\n");
}

TEST(ParserTest, BinaryExpression) {
    std::string source = "let x = 10 + 20 * 30;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(let x = (+ 10 (* 20 30)))\n");
}

TEST(ParserTest, GroupingExpression) {
    std::string source = "let x = (10 + 20) * 30;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(let x = (* (group (+ 10 20)) 30))\n");
}

TEST(ParserTest, InvalidStatement) {
    std::string source = "10 + 20;";
    Lexer lexer(source);
    Parser parser(lexer);
    EXPECT_THROW(parser.parse(), std::runtime_error);
}

TEST(ParserTest, LetStatementWithType) {
    std::string source = "let x: int = 10;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(let x : int = 10)\n");
}

TEST(ParserTest, FunctionDeclaration) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(func add(a : int, b : int) -> int { (return (+ a b)) })\n");
}

TEST(ParserTest, MutStatement) {
    std::string source = "mut x = 10;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(result, "(mut x = 10)\n");
}
