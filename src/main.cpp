#include <iostream>
#include <vector>
#include "Lexer.h"
#include "Token.h"

void run(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}

int main() {
    std::string source = R"(
        // This is a comment.
        let five = 5;
        let ten = 10;

        let add = func(x, y) {
            x + y;
        };

        let result = add(five, ten);
        "hello world"
    )";

    run(source);

    return 0;
}
