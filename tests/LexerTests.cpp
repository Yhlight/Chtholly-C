#include <gtest/gtest.h>
#include "Lexer.h"
#include <vector>

using namespace chtholly;

void assertToken(const Token& token, TokenType type, const std::string& lexeme) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
}

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "(){}[],.-+%*;:?";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 16);
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
    assertToken(tokens[10], TokenType::PERCENT, "%");
    assertToken(tokens[11], TokenType::STAR, "*");
    assertToken(tokens[12], TokenType::SEMICOLON, ";");
    assertToken(tokens[13], TokenType::COLON, ":");
    assertToken(tokens[14], TokenType::QUESTION, "?");
    assertToken(tokens[15], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, DoubleCharacterTokens) {
    std::string source = "! != = == > >= < <= & && | || += -= *= /= %= -> :: ++ --";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 22);
    assertToken(tokens[0], TokenType::BANG, "!");
    assertToken(tokens[1], TokenType::BANG_EQUAL, "!=");
    assertToken(tokens[2], TokenType::EQUAL, "=");
    assertToken(tokens[3], TokenType::EQUAL_EQUAL, "==");
    assertToken(tokens[4], TokenType::GREATER, ">");
    assertToken(tokens[5], TokenType::GREATER_EQUAL, ">=");
    assertToken(tokens[6], TokenType::LESS, "<");
    assertToken(tokens[7], TokenType::LESS_EQUAL, "<=");
    assertToken(tokens[8], TokenType::AMPERSAND, "&");
    assertToken(tokens[9], TokenType::AND, "&&");
    assertToken(tokens[10], TokenType::PIPE, "|");
    assertToken(tokens[11], TokenType::OR, "||");
    assertToken(tokens[12], TokenType::PLUS_EQUAL, "+=");
    assertToken(tokens[13], TokenType::MINUS_EQUAL, "-=");
    assertToken(tokens[14], TokenType::STAR_EQUAL, "*=");
    assertToken(tokens[15], TokenType::SLASH_EQUAL, "/=");
    assertToken(tokens[16], TokenType::PERCENT_EQUAL, "%=");
    assertToken(tokens[17], TokenType::ARROW, "->");
    assertToken(tokens[18], TokenType::COLON_COLON, "::");
    assertToken(tokens[19], TokenType::PLUS_PLUS, "++");
    assertToken(tokens[20], TokenType::MINUS_MINUS, "--");
    assertToken(tokens[21], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, StringLiterals) {
    std::string source = "\"hello world\"";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 2);
    assertToken(tokens[0], TokenType::STRING_LITERAL, "\"hello world\"");
    ASSERT_TRUE(std::holds_alternative<std::string>(tokens[0].literal));
    EXPECT_EQ(std::get<std::string>(tokens[0].literal), "hello world");
}

TEST(LexerTest, NumberLiterals) {
    std::string source = "123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    assertToken(tokens[0], TokenType::NUMBER_LITERAL, "123");
    ASSERT_TRUE(std::holds_alternative<long long>(tokens[0].literal));
    EXPECT_EQ(std::get<long long>(tokens[0].literal), 123);

    assertToken(tokens[1], TokenType::NUMBER_LITERAL, "45.67");
    ASSERT_TRUE(std::holds_alternative<double>(tokens[1].literal));
    EXPECT_EQ(std::get<double>(tokens[1].literal), 45.67);
}

TEST(LexerTest, IdentifiersAndKeywords) {
    std::string source = "let my_var = func";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);
    assertToken(tokens[0], TokenType::LET, "let");
    assertToken(tokens[1], TokenType::IDENTIFIER, "my_var");
    assertToken(tokens[2], TokenType::EQUAL, "=");
    assertToken(tokens[3], TokenType::FUNC, "func");
    assertToken(tokens[4], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, Comments) {
    std::string source = "// this is a comment\nlet x = 10; /* another comment */";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    assertToken(tokens[0], TokenType::LET, "let");
    assertToken(tokens[1], TokenType::IDENTIFIER, "x");
    assertToken(tokens[2], TokenType::EQUAL, "=");
    assertToken(tokens[3], TokenType::NUMBER_LITERAL, "10");
    assertToken(tokens[4], TokenType::SEMICOLON, ";");
    assertToken(tokens[5], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, NestedBlockComments) {
    std::string source = "/* outer /* inner */ outer */ let x = 1;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    assertToken(tokens[0], TokenType::LET, "let");
    assertToken(tokens[1], TokenType::IDENTIFIER, "x");
    assertToken(tokens[2], TokenType::EQUAL, "=");
    assertToken(tokens[3], TokenType::NUMBER_LITERAL, "1");
    assertToken(tokens[4], TokenType::SEMICOLON, ";");
    assertToken(tokens[5], TokenType::END_OF_FILE, "");
}

TEST(LexerTest, ComprehensiveTest) {
    std::string source = R"(
        func main(args: array[string]) -> result<void, string> {
            let message = "Hello, World!";
            print(message);
            /* This is a block comment.
               It can span multiple lines.
               /* It can also be nested. */
            */
            let x = 10 + 20.5; // mixed types
            if (x > 30) {
                return result::pass();
            } else {
                return result::fail("x is not greater than 30");
            }
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::FUNC, TokenType::IDENTIFIER, TokenType::LEFT_PAREN, TokenType::IDENTIFIER, TokenType::COLON, TokenType::ARRAY,
        TokenType::LEFT_BRACKET, TokenType::STRING_TYPE, TokenType::RIGHT_BRACKET, TokenType::RIGHT_PAREN, TokenType::ARROW,
        TokenType::RESULT, TokenType::LESS, TokenType::VOID, TokenType::COMMA, TokenType::STRING_TYPE,
        TokenType::GREATER, TokenType::LEFT_BRACE, TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL,
        TokenType::STRING_LITERAL, TokenType::SEMICOLON, TokenType::PRINT, TokenType::LEFT_PAREN, TokenType::IDENTIFIER,
        TokenType::RIGHT_PAREN, TokenType::SEMICOLON, TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL,
        TokenType::NUMBER_LITERAL, TokenType::PLUS, TokenType::NUMBER_LITERAL, TokenType::SEMICOLON, TokenType::IF,
        TokenType::LEFT_PAREN, TokenType::IDENTIFIER, TokenType::GREATER, TokenType::NUMBER_LITERAL, TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACE, TokenType::RETURN, TokenType::RESULT, TokenType::COLON_COLON, TokenType::IDENTIFIER,
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN, TokenType::SEMICOLON, TokenType::RIGHT_BRACE, TokenType::ELSE,
        TokenType::LEFT_BRACE, TokenType::RETURN, TokenType::RESULT, TokenType::COLON_COLON, TokenType::IDENTIFIER,
        TokenType::LEFT_PAREN, TokenType::STRING_LITERAL, TokenType::RIGHT_PAREN, TokenType::SEMICOLON, TokenType::RIGHT_BRACE,
        TokenType::RIGHT_BRACE, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for(size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}

TEST(LexerTest, ModuleAndBuiltinKeywords) {
    std::string source = "import iostream; print(\"hello\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    std::vector<TokenType> expected = {
        TokenType::IMPORT, TokenType::IOSTREAM, TokenType::SEMICOLON,
        TokenType::PRINT, TokenType::LEFT_PAREN, TokenType::STRING_LITERAL, TokenType::RIGHT_PAREN, TokenType::SEMICOLON,
        TokenType::END_OF_FILE
    };
    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}

TEST(LexerTest, FilesystemKeywords) {
    std::string source = "import filesystem; fs_read(\"test.txt\"); fs_write(\"test.txt\", \"hello\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    std::vector<TokenType> expected = {
        TokenType::IMPORT, TokenType::FILESYSTEM, TokenType::SEMICOLON,
        TokenType::FS_READ, TokenType::LEFT_PAREN, TokenType::STRING_LITERAL, TokenType::RIGHT_PAREN, TokenType::SEMICOLON,
        TokenType::FS_WRITE, TokenType::LEFT_PAREN, TokenType::STRING_LITERAL, TokenType::COMMA, TokenType::STRING_LITERAL, TokenType::RIGHT_PAREN, TokenType::SEMICOLON,
        TokenType::END_OF_FILE
    };
    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}
