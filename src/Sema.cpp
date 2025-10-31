#include "Sema.h"
#include <iostream>

namespace chtholly {

Sema::Sema() = default;

bool Sema::hadError() const {
    return errorOccurred;
}

void Sema::analyze(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
}

void Sema::visit(const BlockStmt& stmt) {
    symbolTable.enterScope();
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    symbolTable.exitScope();
}

void Sema::visit(const VarDeclStmt& stmt) {
    std::unique_ptr<Type> declaredType = nullptr;
    if (stmt.type) {
        declaredType = resolveType(*stmt.type);
    }

    std::unique_ptr<Type> initializerType = nullptr;
    if (stmt.initializer) {
        initializerType = stmt.initializer->accept(*this);
    }

    if (declaredType && initializerType) {
        // Later, check if initializerType is assignable to declaredType
    }

    std::unique_ptr<Type> finalType = std::move(declaredType);
    if (!finalType) {
        finalType = std::move(initializerType);
    }

    if (!finalType) {
        error(stmt.name, "Cannot infer type of variable without an initializer or type annotation.");
        return;
    }

    Mutability mutability = (stmt.keyword.type == TokenType::MUT) ? Mutability::Mutable : Mutability::Immutable;

    if (!symbolTable.define(stmt.name.lexeme, std::move(finalType), mutability)) {
        error(stmt.name, "Variable with this name already declared in this scope.");
    }
}

void Sema::visit(const ExprStmt& stmt) {
    stmt.expression->accept(*this);
}

void Sema::visit(const IfStmt& stmt) {
    // To be implemented
}

void Sema::visit(const ForStmt& stmt) {
    // To be implemented
}

void Sema::visit(const FuncDeclStmt& stmt) {
    // To be implemented
}

void Sema::visit(const ReturnStmt& stmt) {
    // To be implemented
}


std::unique_ptr<Type> Sema::visit(const LiteralExpr& expr) {
    switch (expr.token.type) {
        case TokenType::NUMBER: return std::make_unique<IntType>();
        case TokenType::STRING: return std::make_unique<StringType>();
        case TokenType::TRUE:
        case TokenType::FALSE: return std::make_unique<BoolType>();
        default: return nullptr;
    }
}

std::unique_ptr<Type> Sema::visit(const UnaryExpr& expr) {
    return expr.right->accept(*this);
}

std::unique_ptr<Type> Sema::visit(const BinaryExpr& expr) {
    // For now, assume the type of the left operand. This will be improved.
    return expr.left->accept(*this);
}

std::unique_ptr<Type> Sema::visit(const VariableExpr& expr) {
    Symbol* symbol = symbolTable.lookup(expr.name.lexeme);
    if (!symbol) {
        error(expr.name, "Undefined variable.");
        return std::make_unique<IntType>(); // Return a dummy type to avoid crashing
    }
    // This is not correct as it moves the type out of the symbol.
    // For now, we'll clone it. A better approach would be to return a pointer or reference.
    if (auto* intType = dynamic_cast<IntType*>(symbol->type.get())) return std::make_unique<IntType>();
    if (auto* strType = dynamic_cast<StringType*>(symbol->type.get())) return std::make_unique<StringType>();
    if (auto* boolType = dynamic_cast<BoolType*>(symbol->type.get())) return std::make_unique<BoolType>();
    return nullptr;
}

std::unique_ptr<Type> Sema::visit(const GroupingExpr& expr) {
    return expr.expression->accept(*this);
}

std::unique_ptr<Type> Sema::resolveType(const Token& typeToken) {
    if (typeToken.lexeme == "int") return std::make_unique<IntType>();
    if (typeToken.lexeme == "string") return std::make_unique<StringType>();
    if (typeToken.lexeme == "bool") return std::make_unique<BoolType>();
    if (typeToken.lexeme == "void") return std::make_unique<VoidType>();

    error(typeToken, "Unknown type name.");
    return nullptr;
}


void Sema::error(const Token& token, const std::string& message) {
    errorOccurred = true;
    std::cerr << "Error at '" << token.lexeme << "' (line " << token.line << "): " << message << std::endl;
}

} // namespace chtholly
