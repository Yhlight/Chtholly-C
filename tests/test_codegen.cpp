#include <CodeGen.h>
#include <AST.h>
#include <iostream>
#include <vector>
#include <cassert>

void test_codegen_simple_variable_declaration() {
    // let x = 10;
    auto var_decl = std::make_unique<VarDeclStmt>(
        Token{TokenType::IDENTIFIER, "x", 1, 1},
        Token{TokenType::UNKNOWN, "", 0, 0},
        std::make_unique<LiteralExpr>(Token{TokenType::INTEGER_LITERAL, "10", 1, 1}),
        false
    );
    std::vector<std::unique_ptr<Stmt>> statements;
    statements.push_back(std::move(var_decl));

    CodeGen codegen;
    std::string result = codegen.generate(statements);
    std::string expected = "#include <iostream>\n\nconst auto x = 10;\n";
    assert(result == expected);
    std::cout << "Simple variable declaration test passed!" << std::endl;
}

void test_codegen_mutable_variable_declaration() {
    // mut x = 10;
    auto var_decl = std::make_unique<VarDeclStmt>(
        Token{TokenType::IDENTIFIER, "x", 1, 1},
        Token{TokenType::UNKNOWN, "", 0, 0},
        std::make_unique<LiteralExpr>(Token{TokenType::INTEGER_LITERAL, "10", 1, 1}),
        true
    );
    std::vector<std::unique_ptr<Stmt>> statements;
    statements.push_back(std::move(var_decl));

    CodeGen codegen;
    std::string result = codegen.generate(statements);
    std::string expected = "#include <iostream>\n\nauto x = 10;\n";
    assert(result == expected);
    std::cout << "Mutable variable declaration test passed!" << std::endl;
}


void test_codegen_variable_declaration_with_type() {
    // let x: int = 10;
    auto var_decl = std::make_unique<VarDeclStmt>(
        Token{TokenType::IDENTIFIER, "x", 1, 1},
        Token{TokenType::INT, "int", 1, 1},
        std::make_unique<LiteralExpr>(Token{TokenType::INTEGER_LITERAL, "10", 1, 1}),
        false
    );
    std::vector<std::unique_ptr<Stmt>> statements;
    statements.push_back(std::move(var_decl));

    CodeGen codegen;
    std::string result = codegen.generate(statements);
    std::string expected = "#include <iostream>\n\nconst int x = 10;\n";
    assert(result == expected);
    std::cout << "Variable declaration with type test passed!" << std::endl;
}

int main() {
    test_codegen_simple_variable_declaration();
    test_codegen_mutable_variable_declaration();
    test_codegen_variable_declaration_with_type();
    std::cout << "CodeGen tests finished." << std::endl;
    return 0;
}
