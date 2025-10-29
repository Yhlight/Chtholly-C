#include "SemanticAnalyzer.h"
#include <iostream>

namespace Chtholly {

SemanticAnalyzer::SemanticAnalyzer() {}

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
}

std::any SemanticAnalyzer::visitBlockStmt(BlockStmt& stmt) {
    symbolTable.enterScope();
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    symbolTable.exitScope();
    return nullptr;
}

std::any SemanticAnalyzer::visitVarDeclStmt(VarDeclStmt& stmt) {
    Token type = stmt.type;
    if (stmt.initializer) {
        Token initializerType = std::any_cast<Token>(stmt.initializer->accept(*this));
        if (type.type != TokenType::Unknown && type.text != initializerType.text) {
            std::cerr << "Error: Initializer type '" << initializerType.text << "' does not match variable type '" << type.text << "'." << std::endl;
            hadError = true;
        }
        if (type.type == TokenType::Unknown) {
            type = initializerType;
        }
    }

    if (!symbolTable.define(stmt.name.text, type)) {
        std::cerr << "Error: Variable '" << stmt.name.text << "' already declared in this scope." << std::endl;
        hadError = true;
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitAssignExpr(AssignExpr& expr) {
    expr.value->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitBinaryExpr(BinaryExpr& expr) {
    Token leftType = std::any_cast<Token>(expr.left->accept(*this));
    Token rightType = std::any_cast<Token>(expr.right->accept(*this));

    if (leftType.text != rightType.text) {
        std::cerr << "Error: Mismatched types '" << leftType.text << "' and '" << rightType.text << "' in binary expression." << std::endl;
        hadError = true;
    }

    return leftType;
}

std::any SemanticAnalyzer::visitGroupingExpr(GroupingExpr& expr) {
    expr.expression->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitNumberLiteralExpr(NumberLiteralExpr& expr) {
    return expr.number;
}

std::any SemanticAnalyzer::visitBooleanLiteralExpr(BooleanLiteralExpr& expr) {
    return expr.value;
}

std::any SemanticAnalyzer::visitUnaryExpr(UnaryExpr& expr) {
    expr.right->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitVariableExpr(VariableExpr& expr) {
    Symbol* symbol = symbolTable.lookup(expr.name.text);
    if (symbol == nullptr) {
        std::cerr << "Error: Undefined variable '" << expr.name.text << "'." << std::endl;
        hadError = true;
        Token dummy;
        dummy.type = TokenType::Unknown;
        return dummy;
    }
    return symbol->type;
}

std::any SemanticAnalyzer::visitExprStmt(ExprStmt& stmt) {
    stmt.expression->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitIfStmt(IfStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitWhileStmt(WhileStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.body->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitSwitchStmt(SwitchStmt& stmt) {
    stmt.expression->accept(*this);
    for (const auto& caseStmt : stmt.cases) {
        caseStmt->accept(*this);
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitCaseStmt(CaseStmt& stmt) {
    stmt.expression->accept(*this);
    stmt.body->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitBreakStmt(BreakStmt& stmt) {
    return nullptr;
}

std::any SemanticAnalyzer::visitFallthroughStmt(FallthroughStmt& stmt) {
    return nullptr;
}

std::any SemanticAnalyzer::visitFunctionStmt(FunctionStmt& stmt) {
    if (!symbolTable.define(stmt.name.text, stmt.returnType)) {
        std::cerr << "Error: Function '" << stmt.name.text << "' already declared in this scope." << std::endl;
        hadError = true;
    }
    symbolTable.enterScope();
    for (const auto& param : stmt.params) {
        symbolTable.define(param.name.text, param.type);
    }
    stmt.body->accept(*this);
    symbolTable.exitScope();
    return nullptr;
}

std::any SemanticAnalyzer::visitStructStmt(StructStmt& stmt) {
    if (!symbolTable.define(stmt.name.text, stmt.name)) {
        std::cerr << "Error: Struct '" << stmt.name.text << "' already declared in this scope." << std::endl;
        hadError = true;
    }
    symbolTable.enterScope();
    for (const auto& field : stmt.fields) {
        field->accept(*this);
    }
    for (const auto& method : stmt.methods) {
        method->accept(*this);
    }
    symbolTable.exitScope();
    return nullptr;
}

} // namespace Chtholly
