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

std::shared_ptr<Type> Resolver::visit(const StringLiteral& expr) {
    return std::make_shared<PrimitiveType>(PrimitiveType::Kind::String);
}

std::shared_ptr<Type> Resolver::visit(const UnaryExpr& expr) {
    auto rightType = resolve(*expr.right);
    if (expr.op.type == TokenType::MINUS) {
        if (rightType->getKind() != TypeKind::Primitive ||
            (std::static_pointer_cast<PrimitiveType>(rightType)->getPrimitiveKind() != PrimitiveType::Kind::Int &&
             std::static_pointer_cast<PrimitiveType>(rightType)->getPrimitiveKind() != PrimitiveType::Kind::Double)) {
            throw std::runtime_error("Operand must be a number.");
        }
        return rightType;
    }
    // Handle other unary operators...
    return nullptr;
}

std::shared_ptr<Type> Resolver::visit(const BinaryExpr& expr) {
    auto leftType = resolve(*expr.left);
    auto rightType = resolve(*expr.right);

    if (!leftType->isEqual(*rightType)) {
        throw std::runtime_error("Type mismatch in binary expression.");
    }
    // For now, assume the result type is the same as the operand types.
    return leftType;
}

std::shared_ptr<Type> Resolver::visit(const GroupingExpr& expr) {
    return resolve(*expr.expression);
}

std::shared_ptr<Type> Resolver::visit(const VariableExpr& expr) {
    Symbol* symbol = findSymbol(expr.name);
    if (!symbol) {
        throw std::runtime_error("Undefined variable '" + expr.name.lexeme + "'.");
    }
    if (symbol->state == OwnershipState::Moved) {
        throw std::runtime_error("Use of moved value '" + expr.name.lexeme + "'.");
    }
    if (symbol->state == OwnershipState::BorrowedWrite) {
        throw std::runtime_error("Cannot use '" + expr.name.lexeme + "' while it is mutably borrowed.");
    }
    return symbol->type;
}

std::shared_ptr<Type> Resolver::visit(const BorrowExpr& expr) {
    auto borrowedType = resolve(*expr.expression);
    if (auto varExpr = dynamic_cast<const VariableExpr*>(expr.expression.get())) {
        Symbol* symbol = findSymbol(varExpr->name);
        if (symbol) {
            if (expr.isMutable) {
                if (!symbol->isMutable) {
                    throw std::runtime_error("Cannot mutably borrow immutable variable '" + varExpr->name.lexeme + "'.");
                }
                if (symbol->state == OwnershipState::BorrowedRead || symbol->state == OwnershipState::BorrowedWrite) {
                    throw std::runtime_error("Cannot mutably borrow '" + varExpr->name.lexeme + "' as it is already borrowed.");
                }
                symbol->state = OwnershipState::BorrowedWrite;
            } else {
                if (symbol->state == OwnershipState::BorrowedWrite) {
                    throw std::runtime_error("Cannot immutably borrow '" + varExpr->name.lexeme + "' as it is already mutably borrowed.");
                }
                symbol->state = OwnershipState::BorrowedRead;
            }
            symbol->borrowCount++;
            borrowedSymbols_.push_back(symbol);
        }
    }
    return std::make_shared<ReferenceType>(borrowedType, expr.isMutable);
}

std::shared_ptr<Type> Resolver::visit(const CallExpr& expr) {
    auto calleeType = resolve(*expr.callee);
    if (calleeType->getKind() != TypeKind::Function) {
        throw std::runtime_error("Can only call functions and classes.");
    }

    auto funcType = std::static_pointer_cast<FunctionType>(calleeType);
    if (expr.arguments.size() != funcType->parameterTypes.size()) {
        throw std::runtime_error("Expected " + std::to_string(funcType->parameterTypes.size()) +
                               " arguments but got " + std::to_string(expr.arguments.size()) + ".");
    }

    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        auto argType = resolve(*expr.arguments[i]);
        if (!argType->isEqual(*funcType->parameterTypes[i])) {
            throw std::runtime_error("Argument type mismatch.");
        }

        if (auto varExpr = dynamic_cast<const VariableExpr*>(expr.arguments[i].get())) {
            if (argType->getKind() != TypeKind::Primitive && argType->getKind() != TypeKind::Reference) {
                Symbol* argSymbol = findSymbol(varExpr->name);
                if (argSymbol) {
                    argSymbol->state = OwnershipState::Moved;
                }
            }
        }
    }

    return funcType->returnType;
}

std::shared_ptr<Type> Resolver::visit(const AssignExpr& expr) {
    Symbol* symbol = findSymbol(expr.name);
    if (!symbol) {
        throw std::runtime_error("Undefined variable '" + expr.name.lexeme + "'.");
    }
    if (!symbol->isMutable) {
        throw std::runtime_error("Cannot assign to immutable variable '" + expr.name.lexeme + "'.");
    }
    if (symbol->state == OwnershipState::BorrowedRead) {
        throw std::runtime_error("Cannot assign to '" + expr.name.lexeme + "' as it is immutably borrowed.");
    }

    auto valueType = resolve(*expr.value);
    if (!valueType->isEqual(*symbol->type)) {
        throw std::runtime_error("Type mismatch in assignment.");
    }

    if (auto varExpr = dynamic_cast<const VariableExpr*>(expr.value.get())) {
        if (valueType->getKind() != TypeKind::Primitive && valueType->getKind() != TypeKind::Reference) {
            Symbol* valueSymbol = findSymbol(varExpr->name);
            if (valueSymbol) {
                valueSymbol->state = OwnershipState::Moved;
            }
        }
    }

    return symbol->type;
}

void Resolver::visit(const LetStmt& stmt) {
    std::shared_ptr<Type> valueType = nullptr;
    if (stmt.initializer) {
        valueType = resolve(*stmt.initializer);
        if (auto varExpr = dynamic_cast<const VariableExpr*>(stmt.initializer.get())) {
            Symbol* initSymbol = findSymbol(varExpr->name);
            if (initSymbol && initSymbol->type->getKind() != TypeKind::Primitive) {
                if (initSymbol->state == OwnershipState::Moved) {
                    throw std::runtime_error("Use of moved value '" + varExpr->name.lexeme + "'.");
                }
                initSymbol->state = OwnershipState::Moved;
            }
        }
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

    declare(stmt.name, valueType, stmt.isMutable);
    define(stmt.name);
}

void Resolver::visit(const FuncStmt& stmt) {
    auto returnType = stmt.returnType ? resolveType(stmt.returnType) : std::make_shared<PrimitiveType>(PrimitiveType::Kind::Void);

    std::vector<std::shared_ptr<Type>> paramTypes;
    for (const auto& param : stmt.params) {
        paramTypes.push_back(resolveType(param.type));
    }

    auto funcType = std::make_shared<FunctionType>(returnType, paramTypes);

    declare(stmt.name, funcType, false); // Functions are immutable
    define(stmt.name);

    auto oldFunction = currentFunction_;
    currentFunction_ = funcType;

    beginScope();
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        declare(stmt.params[i].name, paramTypes[i], stmt.params[i].isMutable);
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
        if (auto varExpr = dynamic_cast<const VariableExpr*>(stmt.value.get())) {
            Symbol* returnSymbol = findSymbol(varExpr->name);
            if (returnSymbol && returnSymbol->type->getKind() != TypeKind::Primitive) {
                returnSymbol->state = OwnershipState::Moved;
            }
        }
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

void Resolver::visit(const ExprStmt& stmt) {
    resolve(*stmt.expression);
}

void Resolver::beginScope() {
    scopes_.emplace_back();
}

void Resolver::endScope() {
    for (auto* symbol : borrowedSymbols_) {
        symbol->borrowCount--;
        if (symbol->borrowCount == 0) {
            symbol->state = OwnershipState::Owned;
        }
    }
    borrowedSymbols_.clear();
    scopes_.pop_back();
}

void Resolver::declare(const Token& name, std::shared_ptr<Type> type, bool isMutable) {
    if (scopes_.empty()) {
        scopes_.emplace_back();
    }

    if (scopes_.back().count(name.lexeme)) {
        throw std::runtime_error("Variable with this name already declared in this scope.");
    }

    scopes_.back()[name.lexeme] = Symbol{type, isMutable, OwnershipState::Owned};
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

Symbol* Resolver::findSymbol(const Token& name) {
    for (int i = scopes_.size() - 1; i >= 0; i--) {
        if (scopes_[i].count(name.lexeme)) {
            return &scopes_[i][name.lexeme];
        }
    }
    return nullptr;
}
