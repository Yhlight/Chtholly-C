#include "Resolver.h"

namespace chtholly {

Resolver::Resolver() {
    beginScope(); // Global scope
    // Pre-define all built-in functions and modules
    scopes.back()["print"] = true;
    scopes.back()["input"] = true;
    scopes.back()["fs_read"] = true;
    scopes.back()["fs_write"] = true;
    scopes.back()["fs_exists"] = true;
    scopes.back()["fs_is_file"] = true;
    scopes.back()["fs_is_dir"] = true;
    scopes.back()["fs_list_dir"] = true;
    scopes.back()["fs_remove"] = true;
    scopes.back()["meta"] = true;
    scopes.back()["reflect"] = true;
    scopes.back()["util"] = true;
    scopes.back()["operator"] = true;
    scopes.back()["string"] = true;
    scopes.back()["array"] = true;
    scopes.back()["os"] = true;
    scopes.back()["time"] = true;
    scopes.back()["random"] = true;
}

void Resolver::error(const Token& token, const std::string& message) {
    hadError = true;
    std::cerr << "[line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else {
        std::cerr << " at '" << token.lexeme << "'";
    }
    std::cerr << ": " << message << std::endl;
}

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
    if (scopes.empty()) {
        return;
    }

    auto& scope = scopes.back();
    if (scope.count(name.lexeme)) {
        error(name, "Already a variable with this name in this scope.");
    }
    scope[name.lexeme] = false;
}

void Resolver::define(const Token& name) {
    if (scopes.empty()) {
        return;
    }

    scopes.back()[name.lexeme] = true;
}

std::any Resolver::visitBlockStmt(const BlockStmt& stmt) {
    beginScope();
    resolve(stmt.statements);
    endScope();
    return nullptr;
}

std::any Resolver::visitVarStmt(const VarStmt& stmt) {
    declare(stmt.name);
    if (stmt.initializer) {
        resolve(*stmt.initializer);
    }
    define(stmt.name);
    return nullptr;
}

std::any Resolver::visitVariableExpr(const VariableExpr& expr) {
    if (!scopes.empty()) {
        auto& scope = scopes.back();
        auto it = scope.find(expr.name.lexeme);
        if (it != scope.end() && !it->second) {
            error(expr.name, "Can't read local variable in its own initializer.");
            return nullptr; // Return early after finding an error
        }
    }

    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].count(expr.name.lexeme)) {
            // Found the variable, no error.
            return nullptr;
        }
    }
    // If we get here, the variable is not found in any scope.
    error(expr.name, "Undefined variable.");
    return nullptr;
}

std::any Resolver::visitAssignExpr(const AssignExpr& expr) {
    resolve(*expr.value);
    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].count(expr.name.lexeme)) {
            return nullptr;
        }
    }
    error(expr.name, "Undefined variable.");
    return nullptr;
}

void Resolver::resolveFunction(const FunctionStmt& function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const auto& param : function.params) {
        declare(param);
        define(param);
    }
    resolve(function.body->statements);
    endScope();
    currentFunction = enclosingFunction;
}

std::any Resolver::visitFunctionStmt(const FunctionStmt& stmt) {
    declare(stmt.name);
    define(stmt.name);
    resolveFunction(stmt, FunctionType::FUNCTION);
    return nullptr;
}

std::any Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    return nullptr;
}

std::any Resolver::visitIfStmt(const IfStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.thenBranch);
    if (stmt.elseBranch) {
        resolve(*stmt.elseBranch);
    }
    return nullptr;
}

std::any Resolver::visitReturnStmt(const ReturnStmt& stmt) {
    if (currentFunction == FunctionType::NONE) {
        error(stmt.keyword, "Can't return from top-level code.");
    }

    if (stmt.value) {
        resolve(*stmt.value);
    }
    return nullptr;
}

std::any Resolver::visitWhileStmt(const WhileStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.body);
    return nullptr;
}

std::any Resolver::visitForStmt(const ForStmt& stmt) {
    if (stmt.initializer) {
        resolve(*stmt.initializer);
    }
    if (stmt.condition) {
        resolve(*stmt.condition);
    }
    if (stmt.increment) {
        resolve(*stmt.increment);
    }
    resolve(*stmt.body);
    return nullptr;
}

std::any Resolver::visitBinaryExpr(const BinaryExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    return nullptr;
}

std::any Resolver::visitCallExpr(const CallExpr& expr) {
    expr.callee->accept(*this);
    for (const auto& arg : expr.arguments) {
        arg->accept(*this);
    }
    return nullptr;
}

std::any Resolver::visitGroupingExpr(const GroupingExpr& expr) {
    expr.expression->accept(*this);
    return nullptr;
}

std::any Resolver::visitLiteralExpr(const LiteralExpr& expr) {
    return nullptr;
}

std::any Resolver::visitUnaryExpr(const UnaryExpr& expr) {
    expr.right->accept(*this);
    return nullptr;
}

std::any Resolver::visitGetExpr(const GetExpr& expr) {
    expr.object->accept(*this);
    return nullptr;
}

std::any Resolver::visitSetExpr(const SetExpr& expr) {
    expr.value->accept(*this);
    expr.object->accept(*this);
    return nullptr;
}

std::any Resolver::visitSelfExpr(const SelfExpr& expr) {
    if (currentClass == ClassType::NONE) {
        error(expr.keyword, "Can't use 'self' outside of a class.");
        return nullptr;
    }

    return nullptr;
}

std::any Resolver::visitStructStmt(const StructStmt& stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    declare(stmt.name);
    define(stmt.name);

    beginScope();
    scopes.back()["self"] = true;

    for (const auto& method : stmt.methods) {
        FunctionType declaration = FunctionType::FUNCTION;
        resolveFunction(*method, declaration);
    }

    endScope();

    currentClass = enclosingClass;
    return nullptr;
}

} // namespace chtholly
