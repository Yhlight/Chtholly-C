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
