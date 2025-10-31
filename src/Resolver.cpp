#include "Resolver.h"
#include <stdexcept>

// Forward declaration
std::shared_ptr<Type> resolveType(const std::unique_ptr<Type>& typeNode);

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        resolve(*statement);
    }
}

std::shared_ptr<Type> Resolver::visit(const NumericLiteral& expr) {
    // For simplicity, we'll assume all numeric literals are ints for now.
    return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Int);
}

std::shared_ptr<Type> Resolver::visit(const BooleanLiteral& expr) {
    return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Bool);
}

std::shared_ptr<Type> Resolver::visit(const StringLiteral& expr) {
    return std::make_shared<PrimitiveType>(PrimitiveType::Kind::String);
}

std::shared_ptr<Type> Resolver::visit(const UnaryExpr& expr) {
    auto rightType = resolve(*expr.right);

    switch (expr.op.type) {
        case TokenType::MINUS:
            if (rightType->getKind() != TypeKind::Primitive ||
                (std::static_pointer_cast<PrimitiveType>(rightType)->getPrimitiveKind() != PrimitiveType::Kind::Int &&
                 std::static_pointer_cast<PrimitiveType>(rightType)->getPrimitiveKind() != PrimitiveType::Kind::Double)) {
                throw std::runtime_error("Unary '-' operand must be a number.");
            }
            return rightType;
        case TokenType::BANG:
            if (rightType->getKind() != TypeKind::Primitive ||
                std::static_pointer_cast<PrimitiveType>(rightType)->getPrimitiveKind() != PrimitiveType::Kind::Bool) {
                throw std::runtime_error("Unary '!' operand must be a boolean.");
            }
            return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Bool);
        default:
            break;
    }

    // Unreachable
    return nullptr;
}

std::shared_ptr<Type> Resolver::visit(const BinaryExpr& expr) {
    auto leftType = resolve(*expr.left);
    auto rightType = resolve(*expr.right);

    if (!leftType->isEqual(*rightType)) {
        throw std::runtime_error("Type mismatch in binary expression.");
    }

    switch (expr.op.type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
            if (leftType->getKind() != TypeKind::Primitive ||
                (std::static_pointer_cast<PrimitiveType>(leftType)->getPrimitiveKind() != PrimitiveType::Kind::Int &&
                 std::static_pointer_cast<PrimitiveType>(leftType)->getPrimitiveKind() != PrimitiveType::Kind::Double)) {
                throw std::runtime_error("Operands for arithmetic operations must be numbers.");
            }
            return leftType; // Result type is the same as operand types

        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::EQUAL_EQUAL:
        case TokenType::BANG_EQUAL:
            return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Bool);

        default:
            break;
    }

    // Unreachable
    return nullptr;
}

std::shared_ptr<Type> Resolver::visit(const GroupingExpr& expr) {
    return resolve(*expr.expression);
}

std::shared_ptr<Type> Resolver::visit(const VariableExpr& expr) {
    for (int i = scopes_.size() - 1; i >= 0; i--) {
        if (scopes_[i].count(expr.name.lexeme)) {
            return scopes_[i][expr.name.lexeme];
        }
    }
    throw std::runtime_error("Undefined variable '" + expr.name.lexeme + "'.");
}

void Resolver::visit(const LetStmt& stmt) {
    std::shared_ptr<Type> valueType = nullptr;
    if (stmt.initializer) {
        valueType = resolve(*stmt.initializer);
    }

    if (stmt.type) {
        auto annotatedType = resolveType(stmt.type);
        if (valueType && !valueType->isEqual(*annotatedType)) {
            throw std::runtime_error("Initializer type does not match annotated type.");
        }
        valueType = annotatedType;
    }

    if (!valueType) {
        throw std::runtime_error("Variables must be initialized or have a type annotation.");
    }

    declare(stmt.name, valueType);
    define(stmt.name);
}

void Resolver::visit(const FuncStmt& stmt) {
    auto returnType = stmt.returnType ? resolveType(stmt.returnType) : std::make_shared<PrimitiveType>(PrimitiveType::Kind::Void);

    std::vector<std::shared_ptr<Type>> paramTypes;
    for (const auto& param : stmt.params) {
        paramTypes.push_back(resolveType(param.type));
    }

    auto funcType = std::make_shared<FunctionType>(returnType, paramTypes);

    declare(stmt.name, funcType);
    define(stmt.name);

    auto oldFunction = currentFunction_;
    currentFunction_ = funcType;

    beginScope();
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        declare(stmt.params[i].name, paramTypes[i]);
        define(stmt.params[i].name);
    }
    resolve(*stmt.body);
    endScope();

    currentFunction_ = oldFunction;
}

void Resolver::visit(const BlockStmt& stmt) {
    beginScope();
    for (const auto& statement : stmt.statements) {
        resolve(*statement);
    }
    endScope();
}

void Resolver::visit(const ReturnStmt& stmt) {
    if (currentFunction_ == nullptr) {
        throw std::runtime_error("Cannot return from top-level code.");
    }

    auto funcType = std::static_pointer_cast<FunctionType>(currentFunction_);
    if (stmt.value) {
        auto returnType = resolve(*stmt.value);
        if (!returnType->isEqual(*funcType->returnType)) {
            throw std::runtime_error("Return type does not match function's declared return type.");
        }
    } else {
        if (funcType->returnType->getKind() != TypeKind::Primitive ||
            std::static_pointer_cast<PrimitiveType>(funcType->returnType)->getPrimitiveKind() != PrimitiveType::Kind::Void) {
            throw std::runtime_error("Must return a value from a non-void function.");
        }
    }
}

void Resolver::beginScope() {
    scopes_.emplace_back();
}

void Resolver::endScope() {
    scopes_.pop_back();
}

void Resolver::declare(const Token& name, std::shared_ptr<Type> type) {
    if (scopes_.empty()) {
        return;
    }

    if (scopes_.back().count(name.lexeme)) {
        throw std::runtime_error("Variable with this name already declared in this scope.");
    }

    scopes_.back()[name.lexeme] = type;
}

void Resolver::define(const Token& name) {
    // In our simplified model, declaration and definition are combined,
    // so this function is less critical. We'll leave it for structure.
}

void Resolver::resolve(const Stmt& stmt) {
    stmt.accept(*this);
}

std::shared_ptr<Type> Resolver::resolve(const Expr& expr) {
    return expr.accept(*this);
}

std::shared_ptr<Type> resolveType(const std::unique_ptr<Type>& typeNode) {
    if (auto primitiveType = dynamic_cast<PrimitiveType*>(typeNode.get())) {
        return std::make_shared<PrimitiveType>(primitiveType->getPrimitiveKind());
    }
    // Handle other type kinds here...
    return nullptr;
}
