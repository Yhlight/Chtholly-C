#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"

TEST(SwitchStatementTest, CorrectlyParsesSwitchStatement) {
    std::string source =
        "switch (x) {"
        "  case 1: { break; }"
        "  case 2: { fallthrough; }"
        "}";

    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    Chtholly::ASTPrinter printer;
    std::string result = printer.print(statements);

    std::string expected =
        "(switch x { (case 1: { \n(break;)\n}) (case 2: { \n(fallthrough;)\n}) })";
    ASSERT_EQ(result, expected);
}
