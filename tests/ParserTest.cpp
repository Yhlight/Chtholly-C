#include <gtest/gtest.h>
#include "Parser.h"
#include "AstPrinter.h"
#include "Lexer.h"

TEST(ParserTest, LetStatement) {
    std::string source = "let x = 10;";
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::AstPrinter printer;
    std::string result = printer.print(statements);

    ASSERT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "(let x 10.000000)");
}

TEST(ParserTest, BlockStatement) {
    std::string source = "{ let x = 10; let y = 20; }";
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::AstPrinter printer;
    std::string result = printer.print(statements);

    ASSERT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "(block (let x 10.000000)(let y 20.000000))");
}

TEST(ParserTest, IfStatement) {
    std::string source = "if (x > 5) { let y = 10; }";
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::AstPrinter printer;
    std::string result = printer.print(statements);

    ASSERT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "(if (> x 5.000000) (block (let y 10.000000)))");
}

TEST(ParserTest, IfElseStatement) {
    std::string source = "if (x > 5) { let y = 10; } else { let z = 20; }";
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::AstPrinter printer;
    std::string result = printer.print(statements);

    ASSERT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "(if (> x 5.000000) (block (let y 10.000000)) (block (let z 20.000000)))");
}

TEST(ParserTest, WhileStatement) {
    std::string source = "while (x < 10) { x = x + 1; }";
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::AstPrinter printer;
    std::string result = printer.print(statements);

    ASSERT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "(while (< x 10.000000) (block (; (= x (+ x 1.000000)))))");
}
