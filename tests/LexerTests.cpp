#include <gtest/gtest.h>
#include "Lexer.h"

using namespace chtholly;

TEST(LexerTest, SingleTokens) {
    std::string source = "(){},.-+;*/";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 12);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[3].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[4].type, TokenType::COMMA);
    EXPECT_EQ(tokens[5].type, TokenType::DOT);
    EXPECT_EQ(tokens[6].type, TokenType::MINUS);
    EXPECT_EQ(tokens[7].type, TokenType::PLUS);
    EXPECT_EQ(tokens[8].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[9].type, TokenType::STAR);
    EXPECT_EQ(tokens[10].type, TokenType::SLASH);
    EXPECT_EQ(tokens[11].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, DoubleTokens) {
    std::string source = "! != = == > >= < <=";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 9);
    EXPECT_EQ(tokens[0].type, TokenType::BANG);
    EXPECT_EQ(tokens[1].type, TokenType::BANG_EQUAL);
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::EQUAL_EQUAL);
    EXPECT_EQ(tokens[4].type, TokenType::GREATER);
    EXPECT_EQ(tokens[5].type, TokenType::GREATER_EQUAL);
    EXPECT_EQ(tokens[6].type, TokenType::LESS);
    EXPECT_EQ(tokens[7].type, TokenType::LESS_EQUAL);
    EXPECT_EQ(tokens[8].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, Literals) {
    std::string source = "\"hello\" 123.45 true false";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(std::get<std::string>(tokens[0].literal), "hello");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(std::get<double>(tokens[1].literal), 123.45);
    EXPECT_EQ(tokens[2].type, TokenType::TRUE);
    EXPECT_EQ(std::get<bool>(tokens[2].literal), true);
    EXPECT_EQ(tokens[3].type, TokenType::FALSE);
    EXPECT_EQ(std::get<bool>(tokens[3].literal), false);
    EXPECT_EQ(tokens[4].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, KeywordsAndIdentifiers) {
    std::string source = "let my_var = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "my_var");
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, Comments) {
    std::string source = R"(
        // this is a comment
        let x = 10; // another comment
        /*
         * multi-line comment
         */
        let y = 20;
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 11);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::NUMBER);
    EXPECT_EQ(std::get<double>(tokens[3].literal), 10);
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, TokenType::LET);
    EXPECT_EQ(tokens[6].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[6].lexeme, "y");
    EXPECT_EQ(tokens[7].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[8].type, TokenType::NUMBER);
    EXPECT_EQ(std::get<double>(tokens[8].literal), 20);
    EXPECT_EQ(tokens[9].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[10].type, TokenType::END_OF_FILE);
}
