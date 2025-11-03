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
