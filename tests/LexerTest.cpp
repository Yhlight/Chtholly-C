#include <gtest/gtest.h>
#include "../src/Lexer.h"
#include "../src/Token.h"
#include <vector>

using namespace chtholly;

void assertToken(const Token& token, TokenType type, const std::string& lexeme) {
    ASSERT_EQ(token.type, type);
    ASSERT_EQ(token.lexeme, lexeme);
}

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "(){}[],.-+* /%;:&?";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 18);
    assertToken(tokens[0], TokenType::LEFT_PAREN, "(");
    assertToken(tokens[1], TokenType::RIGHT_PAREN, ")");
    assertToken(tokens[2], TokenType::LEFT_BRACE, "{");
    assertToken(tokens[3], TokenType::RIGHT_BRACE, "}");
    assertToken(tokens[4], TokenType::LEFT_BRACKET, "[");
    assertToken(tokens[5], TokenType::RIGHT_BRACKET, "]");
    assertToken(tokens[6], TokenType::COMMA, ",");
    assertToken(tokens[7], TokenType::DOT, ".");
    assertToken(tokens[8], TokenType::MINUS, "-");
    assertToken(tokens[9], TokenType::PLUS, "+");
    assertToken(tokens[10], TokenType::STAR, "*");
    assertToken(tokens[11], TokenType::SLASH, "/");
    assertToken(tokens[12], TokenType::MODULO, "%");
    assertToken(tokens[13], TokenType::SEMICOLON, ";");
    assertToken(tokens[14], TokenType::COLON, ":");
    assertToken(tokens[15], TokenType::AMPERSAND, "&");
    assertToken(tokens[16], TokenType::QUESTION, "?");
    assertToken(tokens[17], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, DoubleCharacterTokens) {
    std::string source = "! != = == > >= < <= -- ++ -= += *= /= %= -> ::";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 18);
    assertToken(tokens[0], TokenType::BANG, "!");
    assertToken(tokens[1], TokenType::BANG_EQUAL, "!=");
    assertToken(tokens[2], TokenType::EQUAL, "=");
    assertToken(tokens[3], TokenType::EQUAL_EQUAL, "==");
    assertToken(tokens[4], TokenType::GREATER, ">");
    assertToken(tokens[5], TokenType::GREATER_EQUAL, ">=");
    assertToken(tokens[6], TokenType::LESS, "<");
    assertToken(tokens[7], TokenType::LESS_EQUAL, "<=");
    assertToken(tokens[8], TokenType::MINUS_MINUS, "--");
    assertToken(tokens[9], TokenType::PLUS_PLUS, "++");
    assertToken(tokens[10], TokenType::MINUS_EQUAL, "-=");
    assertToken(tokens[11], TokenType::PLUS_EQUAL, "+=");
    assertToken(tokens[12], TokenType::STAR_EQUAL, "*=");
    assertToken(tokens[13], TokenType::SLASH_EQUAL, "/=");
    assertToken(tokens[14], TokenType::MODULO_EQUAL, "%=");
    assertToken(tokens[15], TokenType::ARROW, "->");
    assertToken(tokens[16], TokenType::COLON_COLON, "::");
    assertToken(tokens[17], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, Literals) {
    std::string source = "\"hello\" 123.45 99";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 4);
    assertToken(tokens[0], TokenType::STRING, "\"hello\"");
    ASSERT_EQ(std::get<std::string>(tokens[0].literal), "hello");

    assertToken(tokens[1], TokenType::NUMBER, "123.45");
    ASSERT_EQ(std::get<double>(tokens[1].literal), 123.45);

    assertToken(tokens[2], TokenType::NUMBER, "99");
    ASSERT_EQ(std::get<double>(tokens[2].literal), 99);
    assertToken(tokens[3], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, KeywordsAndIdentifiers) {
    std::string source = "let mut x = if else_if";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 7);
    assertToken(tokens[0], TokenType::LET, "let");
    assertToken(tokens[1], TokenType::MUT, "mut");
    assertToken(tokens[2], TokenType::IDENTIFIER, "x");
    assertToken(tokens[3], TokenType::EQUAL, "=");
    assertToken(tokens[4], TokenType::IF, "if");
    assertToken(tokens[5], TokenType::IDENTIFIER, "else_if");
    assertToken(tokens[6], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, Comments) {
    std::string source = "// this is a comment\nlet x = 5; /* another comment */";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    assertToken(tokens[0], TokenType::LET, "let");
    assertToken(tokens[1], TokenType::IDENTIFIER, "x");
    assertToken(tokens[2], TokenType::EQUAL, "=");
    assertToken(tokens[3], TokenType::NUMBER, "5");
    assertToken(tokens[4], TokenType::SEMICOLON, ";");
    assertToken(tokens[5], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, UnterminatedString) {
    std::string source = "\"hello";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 2);
    ASSERT_EQ(tokens[0].type, TokenType::ERROR);
    assertToken(tokens[1], TokenType::END_OF_FILE, "");
}
