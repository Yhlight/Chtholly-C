#include <gtest/gtest.h>
#include "Lexer.h"

TEST(LexerTest, BasicTokens) {
    std::string source = "let x = 10;";
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, chtholly::TokenType::LET);
    EXPECT_EQ(tokens[1].type, chtholly::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, chtholly::TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, chtholly::TokenType::NUMBER);
    EXPECT_EQ(tokens[4].type, chtholly::TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, chtholly::TokenType::END_OF_FILE);
}
