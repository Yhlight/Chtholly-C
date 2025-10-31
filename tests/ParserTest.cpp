#include <gtest/gtest.h>
#include "Parser.h"
#include "AST.h"

TEST(ParserTest, ParsesNumberLiteral) {
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::NUMBER, "123", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::unique_ptr<chtholly::Expr> expr = parser.parse();

    // Check if the parsed expression is a LiteralExpr
    auto* literalExpr = dynamic_cast<chtholly::LiteralExpr*>(expr.get());
    ASSERT_NE(literalExpr, nullptr);
    EXPECT_EQ(literalExpr->token.lexeme, "123");
    EXPECT_EQ(literalExpr->token.type, chtholly::TokenType::NUMBER);
}
