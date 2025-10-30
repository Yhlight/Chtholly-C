#include <iostream>
#include "Lexer.h"
#include "Token.h"

int main() {
    std::string source = "let x = 10; // This is a comment\nfunc add(a: int, b: int) -> int { return a + b; }";
    Lexer lexer(source);

    Token token;
    do {
        token = lexer.nextToken();
        std::cout << token << std::endl;
    } while (token.type != TokenType::END_OF_FILE);

    return 0;
}
