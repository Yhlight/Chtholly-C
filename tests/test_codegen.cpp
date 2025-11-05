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

void test_codegen_generic_function() {
    // func swap<T>(a: &mut T, b: &mut T) { let temp = a; a = b; b = temp; }
    std::vector<std::unique_ptr<Stmt>> stmts;
    std::vector<Token> templateParams;
    templateParams.push_back({TokenType::IDENTIFIER, "T", 1, 1});
    std::vector<Param> params;
    params.push_back({ {TokenType::IDENTIFIER, "a", 1, 1}, {TokenType::IDENTIFIER, "T", 1, 1}, true, true });
    params.push_back({ {TokenType::IDENTIFIER, "b", 1, 1}, {TokenType::IDENTIFIER, "T", 1, 1}, true, true });
    std::vector<std::unique_ptr<Stmt>> bodyStmts;
    bodyStmts.push_back(std::make_unique<VarDeclStmt>(
        Token{TokenType::IDENTIFIER, "temp", 1, 1},
        Token{TokenType::UNKNOWN, "", 0, 0},
        std::make_unique<VariableExpr>(Token{TokenType::IDENTIFIER, "a", 1, 1}),
        false
    ));
    bodyStmts.push_back(std::make_unique<ExpressionStmt>(
        std::make_unique<AssignmentExpr>(
            Token{TokenType::IDENTIFIER, "a", 1, 1},
            std::make_unique<VariableExpr>(Token{TokenType::IDENTIFIER, "b", 1, 1})
        )
    ));
    bodyStmts.push_back(std::make_unique<ExpressionStmt>(
        std::make_unique<AssignmentExpr>(
            Token{TokenType::IDENTIFIER, "b", 1, 1},
            std::make_unique<VariableExpr>(Token{TokenType::IDENTIFIER, "temp", 1, 1})
        )
    ));
    auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
    stmts.push_back(std::make_unique<FunctionStmt>(
        Token{TokenType::IDENTIFIER, "swap", 1, 1},
        std::move(templateParams),
        std::move(params),
        std::move(body)
    ));

    CodeGen codegen;
    std::string result = codegen.generate(stmts);
    std::string expected = "#include <iostream>\n\ntemplate<typename T>\nauto swap(T& a, T& b) {\nconst auto temp = a;\na = b;\nb = temp;\n}\n";
    assert(result == expected);
    std::cout << "Generic function test passed!" << std::endl;
}

void test_codegen_generic_struct() {
    // struct Point<T> { x: T, y: T, }
    std::vector<std::unique_ptr<Stmt>> stmts;
    std::vector<Token> templateParams;
    templateParams.push_back({TokenType::IDENTIFIER, "T", 1, 1});
    std::vector<std::unique_ptr<VarDeclStmt>> fields;
    fields.push_back(std::make_unique<VarDeclStmt>(
        Token{TokenType::IDENTIFIER, "x", 1, 1},
        Token{TokenType::IDENTIFIER, "T", 1, 1},
        nullptr,
        false
    ));
    fields.push_back(std::make_unique<VarDeclStmt>(
        Token{TokenType::IDENTIFIER, "y", 1, 1},
        Token{TokenType::IDENTIFIER, "T", 1, 1},
        nullptr,
        false
    ));
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    stmts.push_back(std::make_unique<StructStmt>(
        Token{TokenType::IDENTIFIER, "Point", 1, 1},
        std::move(templateParams),
        std::move(fields),
        std::move(methods)
    ));

    CodeGen codegen;
    std::string result = codegen.generate(stmts);
    std::string expected = "#include <iostream>\n\ntemplate<typename T>\nstruct Point {\nT x;\nT y;\n};\n";
    assert(result == expected);
    std::cout << "Generic struct test passed!" << std::endl;
}

int main() {
    test_codegen_simple_variable_declaration();
    test_codegen_mutable_variable_declaration();
    test_codegen_variable_declaration_with_type();
    test_codegen_generic_function();
    test_codegen_generic_struct();
    std::cout << "CodeGen tests finished." << std::endl;
    return 0;
}
