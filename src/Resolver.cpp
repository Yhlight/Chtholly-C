#include "Resolver.h"
#include <stdexcept>

// Forward declaration
std::shared_ptr<Type> resolveType(const std::unique_ptr<Type>& typeNode);

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        if (dynamic_cast<FuncStmt*>(statement.get()) == nullptr) {
            resolve(*statement);
        }
    }

    for (const auto& statement : statements) {
        if (dynamic_cast<FuncStmt*>(statement.get()) != nullptr) {
            resolve(*statement);
        }
    }
}

std::shared_ptr<Type> Resolver::visit(const NumericLiteral& expr) {
    // For simplicity, we'll assume all numeric literals are ints for now.
    return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Int);
}

std::shared_ptr<Type> Resolver::visit(const StringLiteral& expr) {
    return std::make_shared<PrimitiveType>(PrimitiveType::Kind::String);
}

std::shared_ptr<Type> Resolver::visit(const BooleanLiteral& expr) {
    return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Bool);
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
    } else if (expr.op.type == TokenType::BANG) {
        if (rightType->getKind() != TypeKind::Primitive ||
            std::static_pointer_cast<PrimitiveType>(rightType)->getPrimitiveKind() != PrimitiveType::Kind::Bool) {
            throw std::runtime_error("Operand must be a boolean.");
        }
        return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Bool);
    }
    return nullptr; // Should not be reached
}

std::shared_ptr<Type> Resolver::visit(const BinaryExpr& expr) {
    auto leftType = resolve(*expr.left);
    auto rightType = resolve(*expr.right);

    if (leftType->isEqual(*rightType)) {
        switch (expr.op.type) {
            case TokenType::MINUS:
            case TokenType::STAR:
            case TokenType::SLASH:
            case TokenType::PLUS:
                if (leftType->getKind() != TypeKind::Primitive ||
                    (std::static_pointer_cast<PrimitiveType>(leftType)->getPrimitiveKind() != PrimitiveType::Kind::Int &&
                     std::static_pointer_cast<PrimitiveType>(leftType)->getPrimitiveKind() != PrimitiveType::Kind::Double)) {
                    throw std::runtime_error("Operands must be numbers.");
                }
                return leftType;
            case TokenType::GREATER:
            case TokenType::GREATER_EQUAL:
            case TokenType::LESS:
            case TokenType::LESS_EQUAL:
                if (leftType->getKind() != TypeKind::Primitive ||
                    (std::static_pointer_cast<PrimitiveType>(leftType)->getPrimitiveKind() != PrimitiveType::Kind::Int &&
                     std::static_pointer_cast<PrimitiveType>(leftType)->getPrimitiveKind() != PrimitiveType::Kind::Double)) {
                    throw std::runtime_error("Operands must be numbers.");
                }
                return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Bool);
            case TokenType::EQUAL_EQUAL:
            case TokenType::BANG_EQUAL:
                return std::make_shared<PrimitiveType>(PrimitiveType::Kind::Bool);
            default:
                break;
        }
    }

    throw std::runtime_error("Invalid operands for binary expression.");
}

std::shared_ptr<Type> Resolver::visit(const GroupingExpr& expr) {
    return resolve(*expr.expression);
}

std::shared_ptr<Type> Resolver::visit(const BorrowExpr& expr) {
    auto referencedType = resolve(*expr.expression);
    if (auto varExpr = dynamic_cast<const VariableExpr*>(expr.expression.get())) {
        for (int i = scopes_.size() - 1; i >= 0; i--) {
            if (scopes_[i].count(varExpr->name.lexeme)) {
                Symbol& symbol = scopes_[i][varExpr->name.lexeme];
                if (expr.isMutable) {
                    if (!symbol.isMutable) {
                        throw std::runtime_error("Cannot mutably borrow immutable variable '" + varExpr->name.lexeme + "'.");
                    }
                    if (symbol.state == OwnershipState::BorrowedImmutable || symbol.state == OwnershipState::BorrowedMutable) {
                        throw std::runtime_error("Cannot mutably borrow '" + varExpr->name.lexeme + "' as it is already borrowed.");
                    }
                    symbol.state = OwnershipState::BorrowedMutable;
                } else {
                    if (symbol.state == OwnershipState::BorrowedMutable) {
                        throw std::runtime_error("Cannot immutably borrow '" + varExpr->name.lexeme + "' as it is already borrowed as mutable.");
                    }
                    symbol.state = OwnershipState::BorrowedImmutable;
                }
                break;
            }
        }
    }
    return std::make_shared<ReferenceType>(referencedType, expr.isMutable);
}

std::shared_ptr<Type> Resolver::visit(const VariableExpr& expr) {
    for (int i = scopes_.size() - 1; i >= 0; i--) {
        if (scopes_[i].count(expr.name.lexeme)) {
            Symbol& symbol = scopes_[i][expr.name.lexeme];
            if (symbol.state == OwnershipState::Moved) {
                throw std::runtime_error("Use of moved value '" + expr.name.lexeme + "'.");
            }
            if (symbol.type->getKind() != TypeKind::Reference) {
                symbol.state = OwnershipState::Moved;
            }
            return symbol.type;
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

    declare(stmt.name, {valueType, OwnershipState::Owned, stmt.isMutable});
    define(stmt.name);
}

void Resolver::visit(const FuncStmt& stmt) {
    auto returnType = stmt.returnType ? resolveType(stmt.returnType) : std::make_shared<PrimitiveType>(PrimitiveType::Kind::Void);

    std::vector<std::shared_ptr<Type>> paramTypes;
    for (const auto& param : stmt.params) {
        paramTypes.push_back(resolveType(param.type));
    }

    auto funcType = std::make_shared<FunctionType>(returnType, paramTypes);

    declare(stmt.name, {funcType, OwnershipState::Owned});
    define(stmt.name);

    auto oldFunction = currentFunction_;
    currentFunction_ = funcType;

    beginScope();
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        declare(stmt.params[i].name, {paramTypes[i], OwnershipState::Owned});
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

void Resolver::declare(const Token& name, Symbol symbol) {
    if (scopes_.empty()) {
        if (globalScope_.count(name.lexeme)) {
            throw std::runtime_error("Variable with this name already declared in this scope.");
        }
        globalScope_[name.lexeme] = symbol;
        return;
    }

    if (scopes_.back().count(name.lexeme)) {
        throw std::runtime_error("Variable with this name already declared in this scope.");
    }

    scopes_.back()[name.lexeme] = symbol;
}

void Resolver::define(const Token& name) {
    // In our simplified model, declaration and definition are combined,
    // so this function is less critical. We'll leave it for structure.
}

void Resolver::visit(const StructStmt& stmt) {
    std::unordered_map<std::string, StructType::Field> fields;
    for (const auto& field : stmt.fields) {
        fields[field.name.lexeme] = {resolveType(field.type), field.isPublic};
    }
    auto structType = std::make_shared<StructType>(stmt.name.lexeme, fields);
    declare(stmt.name, {structType, OwnershipState::Owned, false});
    define(stmt.name);
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

std::shared_ptr<Type> Resolver::visit(const GetExpr& expr) {
    auto objectType = resolve(*expr.object);
    auto structType = std::dynamic_pointer_cast<StructType>(objectType);
    if (!structType) {
        throw std::runtime_error("Can only access fields of a struct.");
    }

    const auto& fields = structType->getFields();
    if (fields.find(expr.name.lexeme) == fields.end()) {
        throw std::runtime_error("Struct '" + structType->name + "' has no field named '" + expr.name.lexeme + "'.");
    }

    const auto& field = fields.at(expr.name.lexeme);
    if (!field.isPublic) {
        throw std::runtime_error("Cannot access private field '" + expr.name.lexeme + "' of struct '" + structType->name + "'.");
    }

    return field.type;
}

std::shared_ptr<Type> Resolver::visit(const SetExpr& expr) {
    auto objectType = resolve(*expr.object);
    auto structType = std::dynamic_pointer_cast<StructType>(objectType);
    if (!structType) {
        throw std::runtime_error("Can only access fields of a struct.");
    }

    const auto& fields = structType->getFields();
    if (fields.find(expr.name.lexeme) == fields.end()) {
        throw std::runtime_error("Struct '" + structType->name + "' has no field named '" + expr.name.lexeme + "'.");
    }

    const auto& field = fields.at(expr.name.lexeme);
    if (!field.isPublic) {
        throw std::runtime_error("Cannot access private field '" + expr.name.lexeme + "' of struct '" + structType->name + "'.");
    }

    auto valueType = resolve(*expr.value);
    if (!valueType->isEqual(*field.type)) {
        throw std::runtime_error("Type mismatch in field assignment for '" + expr.name.lexeme + "'.");
    }

    return valueType;
}

std::shared_ptr<Type> Resolver::visit(const StructInitializerExpr& expr) {
    Symbol symbol;
    bool found = false;
    if (globalScope_.count(expr.name.lexeme)) {
        symbol = globalScope_[expr.name.lexeme];
        found = true;
    } else {
        for (int i = scopes_.size() - 1; i >= 0; i--) {
            if (scopes_[i].count(expr.name.lexeme)) {
                symbol = scopes_[i][expr.name.lexeme];
                found = true;
                break;
            }
        }
    }
    if (!found) {
        throw std::runtime_error("Undefined struct '" + expr.name.lexeme + "'.");
    }

    auto structType = std::dynamic_pointer_cast<StructType>(symbol.type);
    if (!structType) {
        throw std::runtime_error("'" + expr.name.lexeme + "' is not a struct.");
    }

    const auto& fields = structType->getFields();
    if (expr.fields.size() != fields.size()) {
        throw std::runtime_error("Incorrect number of fields in initializer for struct '" + expr.name.lexeme + "'.");
    }

    for (const auto& field : expr.fields) {
        if (fields.find(field.name.lexeme) == fields.end()) {
            throw std::runtime_error("Struct '" + expr.name.lexeme + "' has no field named '" + field.name.lexeme + "'.");
        }
        auto fieldType = fields.at(field.name.lexeme).type;
        auto initializerType = resolve(*field.initializer);
        if (!initializerType->isEqual(*fieldType)) {
            throw std::runtime_error("Type mismatch in field initializer for '" + field.name.lexeme + "'.");
        }
    }

    return structType;
}
