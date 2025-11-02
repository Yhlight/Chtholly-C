#include <gtest/gtest.h>
#include "../src/Lexer.h"
#include "../src/Token.h"

TEST(LexerTest, BasicTokens) {
    std::string source = "let x = 5;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::NUMBER);
    EXPECT_EQ(std::get<double>(tokens[3].literal), 5);
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, TwoCharacterTokens) {
    std::string source = "!= == <= >=";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::BANG_EQUAL);
    EXPECT_EQ(tokens[1].type, TokenType::EQUAL_EQUAL);
    EXPECT_EQ(tokens[2].type, TokenType::LESS_EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::GREATER_EQUAL);
    EXPECT_EQ(tokens[4].type, TokenType::END_OF_FILE);
}
