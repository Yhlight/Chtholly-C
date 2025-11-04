#include <Parser.h>
#include <Lexer.h>
#include <CodeGen.h>
#include <iostream>
#include <cassert>

void test_borrow_expression() {
    std::string source = "let a = 10; let b = &a;";
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

    CodeGen codegen;
    std::string result = codegen.generate(statements);
    std::string expected = "#include <iostream>\n\nconst auto a = 10;\nconst auto b = &a;\n";
    assert(result == expected);
    std::cout << "Borrow expression test passed!" << std::endl;
}

int main() {
    test_borrow_expression();
    return 0;
}
