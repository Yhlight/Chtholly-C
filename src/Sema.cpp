#include "Sema.h"

namespace chtholly {

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
    std::unique_ptr<Type> type = nullptr;
    if (stmt.initializer) {
        // Type inference will be implemented later
        type = stmt.initializer->accept(*this);
    }

    // For now, let's assume a default type if not inferred
    if (!type) {
        type = std::make_unique<IntType>(); // Placeholder
    }

    // This logic needs to be updated when `mut` is parsed.
    // Assuming `let` for now.
    symbolTable.define(stmt.name.lexeme, std::move(type), Mutability::Immutable);
}

std::unique_ptr<Type> Sema::visit(const LiteralExpr& expr) {
    // Basic type inference for literals
    switch (expr.token.type) {
        case TokenType::NUMBER:
            return std::make_unique<IntType>();
        case TokenType::STRING:
            return std::make_unique<StringType>();
        case TokenType::TRUE:
        case TokenType::FALSE:
            return std::make_unique<BoolType>();
        default:
            return nullptr;
    }
}

std::unique_ptr<Type> Sema::visit(const UnaryExpr& expr) {
    // Placeholder
    return nullptr;
}

std::unique_ptr<Type> Sema::visit(const BinaryExpr& expr) {
    // Placeholder
    return nullptr;
}

std::unique_ptr<Type> Sema::visit(const VariableExpr& expr) {
    // Placeholder
    return nullptr;
}

std::unique_ptr<Type> Sema::visit(const GroupingExpr& expr) {
    return expr.expression->accept(*this);
}

} // namespace chtholly
