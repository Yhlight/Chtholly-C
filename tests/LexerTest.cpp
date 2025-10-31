#include <gtest/gtest.h>
#include "Lexer.h"

TEST(LexerTest, HandlesEmptySource) {
    chtholly::Lexer lexer("");
    chtholly::Token token = lexer.scanToken();
    EXPECT_EQ(token.type, chtholly::TokenType::END_OF_FILE);
}

TEST(LexerTest, ScansSingleCharacterTokens) {
    chtholly::Lexer lexer("(){};,.-+/*");
    std::vector<chtholly::TokenType> expected_types = {
        chtholly::TokenType::LEFT_PAREN,
        chtholly::TokenType::RIGHT_PAREN,
        chtholly::TokenType::LEFT_BRACE,
        chtholly::TokenType::RIGHT_BRACE,
        chtholly::TokenType::SEMICOLON,
        chtholly::TokenType::COMMA,
        chtholly::TokenType::DOT,
        chtholly::TokenType::MINUS,
        chtholly::TokenType::PLUS,
        chtholly::TokenType::SLASH,
        chtholly::TokenType::STAR,
        chtholly::TokenType::END_OF_FILE,
    };

    for (const auto& expected_type : expected_types) {
        chtholly::Token token = lexer.scanToken();
        EXPECT_EQ(token.type, expected_type);
    }
}
