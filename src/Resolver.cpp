#include "Resolver.h"
#include "Error.h"

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        if(statement) resolve(*statement);
    }
}

void Resolver::resolve(const Stmt& stmt) {
    stmt.accept(*this);
}

void Resolver::resolve(const Expr& expr) {
    expr.accept(*this);
}

// Scope management
void Resolver::beginScope() {
    scopes.emplace_back();
}

void Resolver::endScope() {
    scopes.pop_back();
}

void Resolver::declare(const Token& name) {
    if (scopes.empty()) return;

    auto& scope = scopes.back();
    if (scope.count(name.lexeme)) {
        ErrorReporter::error(name.line, "Already a variable with this name in this scope.");
    }
    scope[name.lexeme] = false;
}

void Resolver::define(const Token& name) {
    if (scopes.empty()) return;
    scopes.back()[name.lexeme] = true;
}


// Visitor implementations
std::any Resolver::visitBlockStmt(const BlockStmt& stmt) {
    beginScope();
    resolve(stmt.statements);
    endScope();
    return {};
}

std::any Resolver::visitIfStmt(const IfStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.thenBranch);
    if (stmt.elseBranch) resolve(*stmt.elseBranch);
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

std::any Resolver::visitWhileStmt(const WhileStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.body);
    return {};
}


std::any Resolver::visitVariableExpr(const VariableExpr& expr) {
    if (!scopes.empty()) {
        auto& scope = scopes.back();
        if (scope.count(expr.name.lexeme) && !scope[expr.name.lexeme]) {
            ErrorReporter::error(expr.name.line, "Can't read local variable in its own initializer.");
        }
    }
    // In a later step, we'll look up the variable and mark it as "used".
    return {};
}

std::any Resolver::visitAssignExpr(const AssignExpr& expr) {
    resolve(*expr.value);
    // In a later step, we'll resolve the variable being assigned to.
    return {};
}


// Other visitor methods (just traverse)
std::any Resolver::visitBinaryExpr(const BinaryExpr& expr) {
    resolve(*expr.left);
    resolve(*expr.right);
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

std::any Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    resolve(*stmt.expression);
    return {};
}

std::any Resolver::visitStructStmt(const StructStmt& stmt) {
    declare(stmt.name);
    define(stmt.name);
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

std::any Resolver::visitFuncStmt(const FuncStmt& stmt) {
    declare(stmt.name);
    define(stmt.name);
    resolveFunction(stmt);
    return {};
}

void Resolver::resolveFunction(const FuncStmt& function) {
    bool previousInFunction = inFunction;
    inFunction = true;
    beginScope();
    for (const auto& param : function.params) {
        declare(param);
        define(param);
    }
    resolve(function.body->statements);
    endScope();
    inFunction = previousInFunction;
}

std::any Resolver::visitReturnStmt(const ReturnStmt& stmt) {
    if (!inFunction) {
        ErrorReporter::error(stmt.keyword.line, "Cannot return from top-level code.");
    }
    if (stmt.value) {
        resolve(*stmt.value);
    }
    return {};
}

std::any Resolver::visitCallExpr(const CallExpr& expr) {
    resolve(*expr.callee);
    for (const auto& arg : expr.arguments) {
        resolve(*arg);
    }
    return {};
}

std::any Resolver::visitSwitchStmt(const SwitchStmt& stmt) {
    resolve(*stmt.expression);
    bool previousInSwitch = inSwitch;
    inSwitch = true;
    for (const auto& caseStmt : stmt.cases) {
        resolve(*caseStmt);
    }
    inSwitch = previousInSwitch;
    return {};
}

std::any Resolver::visitCaseStmt(const CaseStmt& stmt) {
    if (stmt.condition) {
        resolve(*stmt.condition);
    }
    resolve(*stmt.body);
    return {};
}

std::any Resolver::visitBreakStmt(const BreakStmt& stmt) {
    if (!inSwitch) {
        ErrorReporter::error(stmt.keyword.line, "Cannot use 'break' outside of a switch statement.");
    }
    return {};
}

std::any Resolver::visitFallthroughStmt(const FallthroughStmt& stmt) {
    if (!inSwitch) {
        ErrorReporter::error(stmt.keyword.line, "Cannot use 'fallthrough' outside of a switch statement.");
    }
    return {};
}

std::any Resolver::visitPrintStmt(const PrintStmt& stmt) {
    resolve(*stmt.expression);
    return {};
}
