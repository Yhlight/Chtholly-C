#include <CodeGen.h>
#include <Lexer.h>
#include <Parser.h>
#include <SemanticAnalyzer.h>
#include <iostream>
#include <vector>
#include <cassert>

void test_function_declaration() {
    std::string source = "func add(a: int, b: int) { return a + b; }";
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

    SemanticAnalyzer analyzer;
    analyzer.analyze(statements);

    CodeGen codegen;
    std::string result = codegen.generate(statements);
    std::string expected = "#include <iostream>\n\nauto add(int a, int b) {\nreturn (a + b);\n}\n";
    assert(result == expected);
    std::cout << "Function declaration test passed!" << std::endl;
}

void test_function_call() {
    std::string source = "func add(a: int, b: int) { return a + b; } add(1, 2);";
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

    SemanticAnalyzer analyzer;
    analyzer.analyze(statements);

    CodeGen codegen;
    std::string result = codegen.generate(statements);
    std::string expected = "#include <iostream>\n\nauto add(int a, int b) {\nreturn (a + b);\n}\nadd(1, 2);\n";
    assert(result == expected);
    std::cout << "Function call test passed!" << std::endl;
}

int main() {
    test_function_declaration();
    test_function_call();
    std::cout << "Function tests finished." << std::endl;
    return 0;
}
