#include <CodeGen.h>
#include <Lexer.h>
#include <Parser.h>
#include <SemanticAnalyzer.h>
#include <iostream>
#include <vector>
#include <cassert>

void test_if_else_statement() {
    std::string source = "if (true) { let x = 10; } else { let y = 20; }";
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
    std::string expected = "#include <iostream>\n\nif (true) {\nconst auto x = 10;\n}\n else {\nconst auto y = 20;\n}\n";
    assert(result == expected);
    std::cout << "If-else statement test passed!" << std::endl;
}

void test_while_statement() {
    std::string source = "while (true) { let x = 10; }";
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
    std::string expected = "#include <iostream>\n\nwhile (true) {\nconst auto x = 10;\n}\n";
    assert(result == expected);
    std::cout << "While statement test passed!" << std::endl;
}

int main() {
    test_if_else_statement();
    test_while_statement();
    std::cout << "Control flow tests finished." << std::endl;
    return 0;
}
