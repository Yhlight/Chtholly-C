#include <Parser.h>
#include <Lexer.h>
#include <AST.h>
#include <iostream>
#include <cassert>

void test_data_type_parsing() {
    std::string source = "let x: int = 5;";
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

    assert(statements.size() == 1);
    VarDeclStmt* varDecl = dynamic_cast<VarDeclStmt*>(statements[0].get());
    assert(varDecl != nullptr);
    assert(varDecl->name.lexeme == "x");
    assert(varDecl->type.lexeme == "int");
    assert(varDecl->initializer != nullptr);
    std::cout << "Data type parsing test passed!" << std::endl;
}

int main() {
    test_data_type_parsing();
    return 0;
}
