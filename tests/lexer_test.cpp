#include "../src/Lexer.h"
#include <vector>
#include <iostream>

void test_lexer() {
    std::string source = "let x = 10; // this is a comment\n func my_func() {}";
    Lexer lexer(source);
    std::vector<Token> expected_tokens = {
        Token::Let, Token::Identifier, Token::Operator, Token::Number, Token::Semicolon,
        Token::Func, Token::Identifier, Token::LeftParen, Token::RightParen,
        Token::LeftBrace, Token::RightBrace, Token::Eof
    };

    for (size_t i = 0; i < expected_tokens.size(); ++i) {
        Token token = lexer.get_token();
        if (token != expected_tokens[i]) {
            std::cerr << "Test failed: Expected " << to_string(expected_tokens[i])
                      << ", but got " << to_string(token) << std::endl;
            exit(1);
        }
    }
    std::cout << "Lexer test passed!" << std::endl;
}

int main() {
    test_lexer();
    return 0;
}
