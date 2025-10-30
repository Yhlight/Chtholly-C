#include "Resolver.h"
#include <stdexcept>

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        resolve(*statement);
    }
}

std::any Resolver::visit(const NumericLiteral& expr) {
    return nullptr;
}

std::any Resolver::visit(const UnaryExpr& expr) {
    resolve(*expr.right);
    return nullptr;
}

std::any Resolver::visit(const BinaryExpr& expr) {
    resolve(*expr.left);
    resolve(*expr.right);
    return nullptr;
}

std::any Resolver::visit(const GroupingExpr& expr) {
    resolve(*expr.expression);
    return nullptr;
}

std::any Resolver::visit(const VariableExpr& expr) {
    if (!scopes_.empty() && scopes_.back().count(expr.name.lexeme) && !scopes_.back().at(expr.name.lexeme)) {
        throw std::runtime_error("Can't read local variable in its own initializer.");
    }
    return nullptr;
}

std::any Resolver::visit(const LetStmt& stmt) {
    declare(stmt.name);
    if (stmt.initializer) {
        resolve(*stmt.initializer);
    }
    define(stmt.name);
    return nullptr;
}

std::any Resolver::visit(const FuncStmt& stmt) {
    declare(stmt.name);
    define(stmt.name);

    beginScope();
    for (const auto& param : stmt.params) {
        declare(param.name);
        define(param.name);
    }
    resolve(*stmt.body);
    endScope();
    return nullptr;
}

std::any Resolver::visit(const BlockStmt& stmt) {
    beginScope();
    for (const auto& statement : stmt.statements) {
        resolve(*statement);
    }
    endScope();
    return nullptr;
}

std::any Resolver::visit(const ReturnStmt& stmt) {
    if (stmt.value) {
        resolve(*stmt.value);
    }
    return nullptr;
}

void Resolver::beginScope() {
    scopes_.emplace_back();
}

void Resolver::endScope() {
    scopes_.pop_back();
}

void Resolver::declare(const Token& name) {
    if (scopes_.empty()) {
        return;
    }

    if (scopes_.back().count(name.lexeme)) {
        throw std::runtime_error("Variable with this name already declared in this scope.");
    }

    scopes_.back()[name.lexeme] = false;
}

void Resolver::define(const Token& name) {
    if (scopes_.empty()) {
        return;
    }
    scopes_.back()[name.lexeme] = true;
}

void Resolver::resolve(const Stmt& stmt) {
    stmt.accept(*this);
}

void Resolver::resolve(const Expr& expr) {
    expr.accept(*this);
}
