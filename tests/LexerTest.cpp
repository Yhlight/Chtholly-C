#include "../src/Lexer.h"
#include <cassert>
#include <vector>

void test_simple_assignment() {
    std::string source = "let a = 10;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens;
    Chtholly::Token token = lexer.nextToken();
    while (token.type != Chtholly::TokenType::Eof) {
        tokens.push_back(token);
        token = lexer.nextToken();
    }

    assert(tokens.size() == 5);
    assert(tokens[0].type == Chtholly::TokenType::Let);
    assert(tokens[1].type == Chtholly::TokenType::Identifier);
    assert(tokens[1].literal == "a");
    assert(tokens[2].type == Chtholly::TokenType::Assign);
    assert(tokens[3].type == Chtholly::TokenType::Integer);
    assert(tokens[3].literal == "10");
    assert(tokens[4].type == Chtholly::TokenType::Semicolon);
}

int main() {
    test_simple_assignment();
    return 0;
}
