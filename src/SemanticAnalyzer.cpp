#include "SemanticAnalyzer.h"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() {
    environment = std::make_shared<Environment>();
}

void SemanticAnalyzer::analyze(const std::vector<std::shared_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        if (statement) {
            analyze(statement);
        }
    }
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors;
}

void SemanticAnalyzer::analyze(const std::shared_ptr<Stmt>& stmt) {
    stmt->accept(*this);
}

void SemanticAnalyzer::analyze(const std::shared_ptr<Expr>& expr) {
    expr->accept(*this);
}

// Statement visitors
void SemanticAnalyzer::visitExpressionStmt(const std::shared_ptr<ExpressionStmt>& stmt) {
    analyze(stmt->expression);
}

void SemanticAnalyzer::visitPrintStmt(const std::shared_ptr<PrintStmt>& stmt) {
    analyze(stmt->expression);
}

void SemanticAnalyzer::visitVarStmt(const std::shared_ptr<VarStmt>& stmt) {
    if (stmt->initializer) {
        analyze(stmt->initializer);
    }
    environment->define(stmt->name.lexeme, stmt->keyword.type == TokenType::MUT);
}

// Expression visitors
std::any SemanticAnalyzer::visitAssignExpr(const std::shared_ptr<Assign>& expr) {
    analyze(expr->value);
    if (!environment->isDefined(expr->name.lexeme)) {
        errors.push_back("Error: Undefined variable '" + expr->name.lexeme + "'.");
    } else if (!environment->isMutable(expr->name.lexeme)) {
        errors.push_back("Error: Cannot assign to immutable variable '" + expr->name.lexeme + "'.");
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitBinaryExpr(const std::shared_ptr<Binary>& expr) {
    analyze(expr->left);
    analyze(expr->right);
    return nullptr;
}

std::any SemanticAnalyzer::visitGroupingExpr(const std::shared_ptr<Grouping>& expr) {
    analyze(expr->expression);
    return nullptr;
}

std::any SemanticAnalyzer::visitLiteralExpr(const std::shared_ptr<Literal>& expr) {
    return nullptr;
}

std::any SemanticAnalyzer::visitUnaryExpr(const std::shared_ptr<Unary>& expr) {
    analyze(expr->right);
    return nullptr;
}

std::any SemanticAnalyzer::visitVariableExpr(const std::shared_ptr<Variable>& expr) {
    if (!environment->isDefined(expr->name.lexeme)) {
        errors.push_back("Error: Undefined variable '" + expr->name.lexeme + "'.");
    }
    return nullptr;
}
