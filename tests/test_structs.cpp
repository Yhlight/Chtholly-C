#include <Parser.h>
#include <Lexer.h>
#include <CodeGen.h>
#include <iostream>
#include <cassert>

void test_struct_declaration() {
    std::string source = "struct Point { x: int, y: int, }";
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
    StructStmt* structStmt = dynamic_cast<StructStmt*>(statements[0].get());
    assert(structStmt != nullptr);
    assert(structStmt->name.lexeme == "Point");
    assert(structStmt->fields.size() == 2);
    assert(structStmt->fields[0]->name.lexeme == "x");
    assert(structStmt->fields[1]->name.lexeme == "y");
    std::cout << "Struct declaration test passed!" << std::endl;
}

void test_struct_codegen() {
    std::string source = "struct Point { x: int, y: int, add(a: int, b: int) {} }";
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
    std::string expected = "#include <iostream>\n\nstruct Point {\nint x;\nint y;\nauto add(int a, int b) {\n}\n};\n";
    assert(result == expected);
    std::cout << "Struct codegen test passed!" << std::endl;
}

int main() {
    test_struct_declaration();
    test_struct_codegen();
    return 0;
}
