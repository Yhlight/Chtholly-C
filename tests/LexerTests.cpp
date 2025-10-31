#include <gtest/gtest.h>
#include "Lexer.h"
#include "Token.h"

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "(){}[],.-+;*/?&:";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 17);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[3].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[4].type, TokenType::LEFT_BRACKET);
    EXPECT_EQ(tokens[5].type, TokenType::RIGHT_BRACKET);
    EXPECT_EQ(tokens[6].type, TokenType::COMMA);
    EXPECT_EQ(tokens[7].type, TokenType::DOT);
    EXPECT_EQ(tokens[8].type, TokenType::MINUS);
    EXPECT_EQ(tokens[9].type, TokenType::PLUS);
    EXPECT_EQ(tokens[10].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[11].type, TokenType::STAR);
    EXPECT_EQ(tokens[12].type, TokenType::SLASH);
    EXPECT_EQ(tokens[13].type, TokenType::QUESTION);
    EXPECT_EQ(tokens[14].type, TokenType::AMPERSAND);
    EXPECT_EQ(tokens[15].type, TokenType::COLON);
    EXPECT_EQ(tokens[16].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, MultiCharacterTokens) {
    std::string source = "! != = == > >= < <= -> ::";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 11);
    EXPECT_EQ(tokens[0].type, TokenType::BANG);
    EXPECT_EQ(tokens[1].type, TokenType::BANG_EQUAL);
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::EQUAL_EQUAL);
    EXPECT_EQ(tokens[4].type, TokenType::GREATER);
    EXPECT_EQ(tokens[5].type, TokenType::GREATER_EQUAL);
    EXPECT_EQ(tokens[6].type, TokenType::LESS);
    EXPECT_EQ(tokens[7].type, TokenType::LESS_EQUAL);
    EXPECT_EQ(tokens[8].type, TokenType::ARROW);
    EXPECT_EQ(tokens[9].type, TokenType::COLON_COLON);
    EXPECT_EQ(tokens[10].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, Literals) {
    std::string source = "\"hello\" 123 45.67 'a'";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "hello");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].lexeme, "123");
    EXPECT_EQ(tokens[2].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[2].lexeme, "45.67");
    EXPECT_EQ(tokens[3].type, TokenType::CHAR);
    EXPECT_EQ(tokens[3].lexeme, "a");
    EXPECT_EQ(tokens[4].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, KeywordsAndIdentifiers) {
    std::string source = "let mut func myVar";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::MUT);
    EXPECT_EQ(tokens[2].type, TokenType::FUNC);
    EXPECT_EQ(tokens[3].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[3].lexeme, "myVar");
    EXPECT_EQ(tokens[4].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, Comments) {
    std::string source = "// this is a comment\nlet x = 10; /* block comment */";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, TokenType::END_OF_FILE);
}
