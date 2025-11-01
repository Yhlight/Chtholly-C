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
    for (const auto& statement : stmt.statements) {
        resolve(statement.get());
    }
    symbolTable_.endScope();
}

void Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    resolve(stmt.expression.get());
}

void Resolver::visitFuncStmt(const FuncStmt& stmt) {
    symbolTable_.declare(stmt.name.lexeme, false, nullptr);
    symbolTable_.define(stmt.name.lexeme);

    symbolTable_.beginScope();
    for (const auto& param : stmt.params) {
        symbolTable_.declare(param.first.lexeme, false,
                              param.second ? std::make_unique<PrimitiveType>(param.second->toString()) : nullptr);
        symbolTable_.define(param.first.lexeme);
    }
    resolve(stmt.body.get());
    symbolTable_.endScope();
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

void Resolver::visitReturnStmt(const ReturnStmt& stmt) {
    if (stmt.value) {
        resolve(stmt.value.get());
    }
}

void Resolver::visitIfStmt(const IfStmt& stmt) {
    resolve(stmt.condition.get());
    resolve(stmt.thenBranch.get());
    if (stmt.elseBranch) {
        resolve(stmt.elseBranch.get());
    }
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

void Resolver::visitCallExpr(const Call& expr) {
    resolve(expr.callee.get());

    for (const auto& argument : expr.arguments) {
        resolve(argument.get());
    }
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
