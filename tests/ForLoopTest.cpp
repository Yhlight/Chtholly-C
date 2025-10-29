#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"

TEST(ForLoopTest, CorrectlyParsesForLoop) {
    std::string source = "for (let i = 0; i < 10; i = i + 1) { }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    Chtholly::ASTPrinter printer;
    std::string result = printer.print(statements);

    std::string expected =
        "{ \n(var i = 0;)\n(while (< i 10) { \n{ \n}(expr (i = (+ i 1)))\n})\n}";
    ASSERT_EQ(result, expected);
}
