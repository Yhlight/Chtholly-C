#include <Parser.h>
#include <Lexer.h>
#include <ASTPrinter.h>
#include <iostream>
#include <cassert>

void test_logical_operator_precedence() {
    std::string source = "1 + 2 * 3 == 7 && 4 + 5 == 9 || 6 + 7 == 13;";
    Lexer lexer(source);
    std::vector<Token> tokens;
    Token token = lexer.nextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = lexer.nextToken();
    }
    tokens.push_back(token);

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);
    std::string expected = "(|| (&& (== (+ 1 (* 2 3)) 7) (== (+ 4 5) 9)) (== (+ 6 7) 13));";
    assert(result == expected);
    std::cout << "Logical operator precedence test passed!" << std::endl;
}

int main() {
    test_logical_operator_precedence();
    return 0;
}
