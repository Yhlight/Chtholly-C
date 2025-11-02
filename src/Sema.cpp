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
    symbol->isMutable = stmt.keyword.type == TokenType::MUT;

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
    std::shared_ptr<Type> resultType;

    switch (expr.op.type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
            if (dynamic_cast<DoubleType*>(leftType.get()) && dynamic_cast<DoubleType*>(rightType.get())) {
                resultType = std::make_shared<DoubleType>();
                return resultType;
            }
            break;
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
            if (dynamic_cast<DoubleType*>(leftType.get()) && dynamic_cast<DoubleType*>(rightType.get())) {
                resultType = std::make_shared<BoolType>();
                return resultType;
            }
            break;
        case TokenType::EQUAL_EQUAL:
        case TokenType::BANG_EQUAL:
            if (dynamic_cast<DoubleType*>(leftType.get()) && dynamic_cast<DoubleType*>(rightType.get())) {
                resultType = std::make_shared<BoolType>();
                return resultType;
            }
            if (dynamic_cast<BoolType*>(leftType.get()) && dynamic_cast<BoolType*>(rightType.get())) {
                resultType = std::make_shared<BoolType>();
                return resultType;
            }
            break;
        case TokenType::AND:
        case TokenType::OR:
            if (dynamic_cast<BoolType*>(leftType.get()) && dynamic_cast<BoolType*>(rightType.get())) {
                resultType = std::make_shared<BoolType>();
                return resultType;
            }
            break;
        default:
            break;
    }

    std::cerr << "Error: Invalid operands for binary expression." << std::endl;
    m_hadError = true;
    resultType = std::make_shared<VoidType>();
    return resultType;
}

std::any Sema::visit(const AssignExpr& expr) {
    auto valueType = std::any_cast<std::shared_ptr<Type>>(expr.value->accept(*this));

    if (auto symbol = symbolTable.lookup(expr.name.lexeme)) {
        if (!symbol->isMutable) {
            std::cerr << "Error: Cannot assign to immutable variable '" << expr.name.lexeme << "'." << std::endl;
            m_hadError = true;
        }

        if (symbol->type->toString() != valueType->toString()) {
            std::cerr << "Error: Type mismatch in assignment to variable '" << expr.name.lexeme << "'." << std::endl;
            m_hadError = true;
        }
    } else {
        std::cerr << "Error: Undefined variable '" << expr.name.lexeme << "'." << std::endl;
        m_hadError = true;
    }

    return valueType;
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

std::any Sema::visit(const IfStmt& stmt) {
    auto conditionType = std::any_cast<std::shared_ptr<Type>>(stmt.condition->accept(*this));

    if (!dynamic_cast<BoolType*>(conditionType.get())) {
        std::cerr << "Error: If condition must be of type bool." << std::endl;
        m_hadError = true;
    }

    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }

    return {};
}

std::any Sema::visit(const WhileStmt& stmt) {
    auto conditionType = std::any_cast<std::shared_ptr<Type>>(stmt.condition->accept(*this));

    if (!dynamic_cast<BoolType*>(conditionType.get())) {
        std::cerr << "Error: While condition must be of type bool." << std::endl;
        m_hadError = true;
    }

    stmt.body->accept(*this);
    return {};
}

} // namespace chtholly