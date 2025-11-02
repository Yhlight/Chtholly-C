#include "Sema.h"
#include "Type.h"
#include <iostream>

namespace chtholly {

Sema::Sema() : m_hadError(false) {}

void Sema::analyze(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        if (statement) {
            statement->accept(*this);
        }
    }
}

bool Sema::hadError() const {
    return m_hadError;
}

std::any Sema::visit(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    return {};
}

std::any Sema::visit(const VarDeclStmt& stmt) {
    std::shared_ptr<Type> type;
    if (stmt.initializer) {
        type = std::any_cast<std::shared_ptr<Type>>(stmt.initializer->accept(*this));
    } else {
        type = std::make_shared<VoidType>();
    }

    auto symbol = std::make_shared<Symbol>();
    symbol->name = stmt.name.lexeme;
    symbol->type = type;
    symbol->isMutable = false;

    if (!symbolTable.define(symbol->name, symbol)) {
        std::cerr << "Error: Variable '" << stmt.name.lexeme << "' already defined in this scope." << std::endl;
        m_hadError = true;
    }
    return {};
}

std::any Sema::visit(const BlockStmt& stmt) {
    symbolTable.enterScope();
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    symbolTable.leaveScope();
    return {};
}

std::any Sema::visit(const BinaryExpr& expr) {
    auto leftType = std::any_cast<std::shared_ptr<Type>>(expr.left->accept(*this));
    auto rightType = std::any_cast<std::shared_ptr<Type>>(expr.right->accept(*this));

    // Numbers are parsed as doubles for now.
    if (dynamic_cast<DoubleType*>(leftType.get()) && dynamic_cast<DoubleType*>(rightType.get())) {
        std::shared_ptr<Type> resultType = std::make_shared<DoubleType>();
        return resultType;
    }

    std::cerr << "Error: Invalid operands for binary expression." << std::endl;
    m_hadError = true;
    std::shared_ptr<Type> resultType = std::make_shared<VoidType>();
    return resultType;
}

std::any Sema::visit(const GroupingExpr& expr) {
    return expr.expression->accept(*this);
}

std::any Sema::visit(const LiteralExpr& expr) {
    std::shared_ptr<Type> type;
    if (std::holds_alternative<double>(expr.value)) {
        type = std::make_shared<DoubleType>();
    } else if (std::holds_alternative<bool>(expr.value)) {
        type = std::make_shared<BoolType>();
    } else if (std::holds_alternative<std::string>(expr.value)) {
        type = std::make_shared<StringType>();
    } else {
        type = std::make_shared<VoidType>();
    }
    return type;
}

std::any Sema::visit(const UnaryExpr& expr) {
    return expr.right->accept(*this);
}

std::any Sema::visit(const VariableExpr& expr) {
    if (auto symbol = symbolTable.lookup(expr.name.lexeme)) {
        return symbol->type;
    }

    std::cerr << "Error: Undefined variable '" << expr.name.lexeme << "'." << std::endl;
    m_hadError = true;
    std::shared_ptr<Type> type = std::make_shared<VoidType>();
    return type;
}

} // namespace chtholly
