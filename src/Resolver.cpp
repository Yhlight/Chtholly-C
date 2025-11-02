#include "Resolver.h"
#include "Error.h"

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        resolve(*statement);
    }
}

void Resolver::resolve(const Stmt& stmt) {
    stmt.accept(*this);
}

void Resolver::resolve(const Expr& expr) {
    expr.accept(*this);
}

void Resolver::beginScope() {
    scopes.push_back({});
}

void Resolver::endScope() {
    scopes.pop_back();
}

void Resolver::declare(const Token& name) {
    if (scopes.empty()) return;
    if (scopes.back().count(name.lexeme)) {
        ErrorReporter::error(name.line, "Already a variable with this name in this scope.");
    }
    scopes.back()[name.lexeme] = false;
}

void Resolver::define(const Token& name) {
    if (scopes.empty()) return;
    scopes.back()[name.lexeme] = true;
}

std::any Resolver::visitBlockStmt(const BlockStmt& stmt) {
    beginScope();
    resolve(stmt.statements);
    endScope();
    return {};
}

std::any Resolver::visitLetStmt(const LetStmt& stmt) {
    declare(stmt.name);
    if (stmt.initializer) {
        resolve(*stmt.initializer);
    }
    define(stmt.name);
    return {};
}

std::any Resolver::visitVariableExpr(const VariableExpr& expr) {
    if (!scopes.empty() && scopes.back().count(expr.name.lexeme) && !scopes.back().at(expr.name.lexeme)) {
        ErrorReporter::error(expr.name.line, "Can't read local variable in its own initializer.");
    }

    resolveLocal(expr, expr.name);
    return {};
}

std::any Resolver::visitAssignExpr(const AssignExpr& expr) {
    resolve(*expr.value);
    resolveLocal(expr, expr.name);
    return {};
}

void Resolver::resolveLocal(const Expr& expr, const Token& name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].count(name.lexeme)) {
            return;
        }
    }

    ErrorReporter::error(name.line, "Use of unresolved variable '" + name.lexeme + "'.");
}

std::any Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    resolve(*stmt.expression);
    return {};
}

std::any Resolver::visitPrintStmt(const PrintStmt& stmt) {
    resolve(*stmt.expression);
    return {};
}

std::any Resolver::visitBinaryExpr(const BinaryExpr& expr) {
    resolve(*expr.left);
    resolve(*expr.right);
    return {};
}

std::any Resolver::visitGetExpr(const GetExpr& expr) {
    resolve(*expr.object);
    return {};
}

std::any Resolver::visitSetExpr(const SetExpr& expr) {
    resolve(*expr.value);
    resolve(*expr.object);
    return {};
}

std::any Resolver::visitStructStmt(const StructStmt& stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;
    declare(stmt.name);
    define(stmt.name);

    beginScope();
    scopes.back()["self"] = true;

    for (const auto& field : stmt.fields) {
        resolve(*field);
    }
    endScope();
    currentClass = enclosingClass;
    return {};
}

std::any Resolver::visitCallExpr(const CallExpr& expr) {
    resolve(*expr.callee);

    for (const auto& argument : expr.arguments) {
        resolve(*argument);
    }

    return {};
}

void Resolver::resolveFunction(const FunctionStmt& function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const Token& param : function.params) {
        declare(param);
        define(param);
    }
    resolve(function.body);
    endScope();
    currentFunction = enclosingFunction;
}

std::any Resolver::visitFunctionStmt(const FunctionStmt& stmt) {
    declare(stmt.name);
    define(stmt.name);
    resolveFunction(stmt, FunctionType::FUNCTION);
    return {};
}

std::any Resolver::visitReturnStmt(const ReturnStmt& stmt) {
    if (currentFunction == FunctionType::NONE) {
        ErrorReporter::error(stmt.keyword.line, "Can't return from top-level code.");
    }

    if (stmt.value) {
        resolve(*stmt.value);
    }

    return {};
}

std::any Resolver::visitIfStmt(const IfStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.thenBranch);
    if (stmt.elseBranch) resolve(*stmt.elseBranch);
    return {};
}

std::any Resolver::visitWhileStmt(const WhileStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.body);
    return {};
}

std::any Resolver::visitGroupingExpr(const GroupingExpr& expr) {
    resolve(*expr.expression);
    return {};
}

std::any Resolver::visitLiteralExpr(const LiteralExpr& expr) {
    return {};
}

std::any Resolver::visitUnaryExpr(const UnaryExpr& expr) {
    resolve(*expr.right);
    return {};
}
