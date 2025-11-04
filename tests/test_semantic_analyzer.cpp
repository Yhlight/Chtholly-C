#include <SemanticAnalyzer.h>
#include <Lexer.h>
#include <Parser.h>
#include <iostream>
#include <vector>
#include <cassert>
#include <sstream>

// Helper to redirect stderr and run analysis
std::string get_semantic_error(const std::string& source) {
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

    std::stringstream buffer;
    std::streambuf* old_stderr = std::cerr.rdbuf(buffer.rdbuf());

    SemanticAnalyzer analyzer;
    analyzer.analyze(statements);

    std::cerr.rdbuf(old_stderr);
    return buffer.str();
}

void test_semantic_analyzer_undeclared_variable() {
    std::string error = get_semantic_error("x = 10;");
    assert(error.find("Undefined variable 'x'") != std::string::npos);
    std::cout << "Undeclared variable test passed!" << std::endl;
}

void test_semantic_analyzer_redefined_variable() {
    std::string error = get_semantic_error("let x = 10; let x = 20;");
    assert(error.find("Variable 'x' already defined in this scope.") != std::string::npos);
    std::cout << "Redefined variable test passed!" << std::endl;
}

void test_semantic_analyzer_assign_to_immutable() {
    std::string error = get_semantic_error("let x = 10; x = 20;");
    assert(error.find("Cannot assign to immutable variable 'x'") != std::string::npos);
    std::cout << "Assign to immutable test passed!" << std::endl;
}

void test_semantic_analyzer_mutable_variable() {
    std::string error = get_semantic_error("mut x = 10; x = 20;");
    assert(error.empty());
    std::cout << "Mutable variable test passed!" << std::endl;
}

int main() {
    test_semantic_analyzer_undeclared_variable();
    test_semantic_analyzer_redefined_variable();
    test_semantic_analyzer_assign_to_immutable();
    test_semantic_analyzer_mutable_variable();
    std::cout << "Semantic analyzer tests finished." << std::endl;
    return 0;
}
