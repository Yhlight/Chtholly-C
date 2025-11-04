#include <Lexer.h>
#include <iostream>
#include <vector>
#include <cassert>

void test_lexer() {
    std::string source = "let x = 10; // this is a comment";
    Lexer lexer(source);
    std::vector<Token> tokens;
    Token token = lexer.nextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = lexer.nextToken();
    }
    tokens.push_back(token); // Add EOF token

    assert(tokens.size() == 6);
    assert(tokens[0].type == TokenType::LET);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[2].type == TokenType::EQUAL);
    assert(tokens[3].type == TokenType::INTEGER_LITERAL);
    assert(tokens[4].type == TokenType::SEMICOLON);
    assert(tokens[5].type == TokenType::END_OF_FILE);

    std::cout << "Lexer test passed!" << std::endl;
}

int main() {
    test_lexer();
    return 0;
}
