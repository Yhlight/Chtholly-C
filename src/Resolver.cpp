#include "Resolver.h"
#include <stdexcept>
#include "Type.h"

Resolver::Resolver() = default;

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        resolve(statement.get());
    }
}

void Resolver::resolve(Stmt* stmt) {
    if (stmt) {
        stmt->accept(*this);
    }
}

void Resolver::resolve(Expr* expr) {
    expr->accept(*this);
}

void Resolver::visitBlockStmt(const Block& stmt) {
    symbolTable_.beginScope();
    resolve(stmt.statements);
    symbolTable_.endScope();
}

void Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    resolve(stmt.expression.get());
}

void Resolver::visitLetStmt(const LetStmt& stmt) {
    if (symbolTable_.isDeclaredInCurrentScope(stmt.name.lexeme)) {
        throw std::runtime_error("Variable with this name already declared in this scope.");
    }
    symbolTable_.declare(stmt.name.lexeme, stmt.isMutable, stmt.type ? std::make_unique<PrimitiveType>(stmt.type->toString()) : nullptr);
    if (stmt.initializer) {
        resolve(stmt.initializer.get());
    }
    symbolTable_.define(stmt.name.lexeme);
}

void Resolver::visitPrintStmt(const PrintStmt& stmt) {
    resolve(stmt.expression.get());
}

void Resolver::visitAssignExpr(const Assign& expr) {
    resolve(expr.value.get());
    if (!symbolTable_.isMutable(expr.name.lexeme)) {
        throw std::runtime_error("Cannot assign to immutable variable '" + expr.name.lexeme + "'.");
    }
}

void Resolver::visitBinaryExpr(const Binary& expr) {
    resolve(expr.left.get());
    resolve(expr.right.get());
}

void Resolver::visitGroupingExpr(const Grouping& expr) {
    resolve(expr.expression.get());
}

void Resolver::visitLiteralExpr(const Literal& expr) {
    // Literals don't need to be resolved.
}

void Resolver::visitUnaryExpr(const Unary& expr) {
    resolve(expr.right.get());
}

void Resolver::visitVariableExpr(const Variable& expr) {
    if (!symbolTable_.isDeclared(expr.name.lexeme)) {
        throw std::runtime_error("Undefined variable '" + expr.name.lexeme + "'.");
    }
}
