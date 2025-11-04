#include <Parser.h>
#include <ASTPrinter.h>
#include <Lexer.h>
#include <iostream>
#include <vector>
#include <cassert>

void test_parser() {
    std::string source = "let x = 10;";
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

    assert(result == "(var x 10);");

    std::cout << "Parser test passed!" << std::endl;
}

int main() {
    test_parser();
    return 0;
}
