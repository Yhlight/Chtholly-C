#include "Sema.h"
#include <iostream>

namespace chtholly {

// Helper to check if two types are the same.
bool areTypesEqual(const Type* a, const Type* b) {
    if (!a || !b) return a == b; // Both null is considered equal.
    if (dynamic_cast<const IntType*>(a) && dynamic_cast<const IntType*>(b)) return true;
    if (dynamic_cast<const StringType*>(a) && dynamic_cast<const StringType*>(b)) return true;
    if (dynamic_cast<const BoolType*>(a) && dynamic_cast<const BoolType*>(b)) return true;
    if (dynamic_cast<const VoidType*>(a) && dynamic_cast<const VoidType*>(b)) return true;
    return false;
}


Sema::Sema() {
    // Start with a null function type.
    currentFunctionType = nullptr;
}

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

    if (declaredType && initializerType && !areTypesEqual(declaredType.get(), initializerType.get())) {
        error(stmt.name, "Initializer type does not match variable's declared type.");
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
    std::unique_ptr<Type> conditionType = stmt.condition->accept(*this);
    if (conditionType && !dynamic_cast<BoolType*>(conditionType.get())) {
        error(stmt.condition->getToken(), "If condition must be a boolean.");
    }
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
}

void Sema::visit(const ForStmt& stmt) {
    symbolTable.enterScope();
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }
    if (stmt.condition) {
        std::unique_ptr<Type> conditionType = stmt.condition->accept(*this);
        if (conditionType && !dynamic_cast<BoolType*>(conditionType.get())) {
            error(stmt.condition->getToken(), "For loop condition must be a boolean.");
        }
    }
    if (stmt.increment) {
        stmt.increment->accept(*this);
    }
    stmt.body->accept(*this);
    symbolTable.exitScope();
}

void Sema::visit(const FuncDeclStmt& stmt) {
    std::unique_ptr<Type> returnType = std::make_unique<VoidType>();
    if (stmt.returnType) {
        returnType = resolveType(*stmt.returnType);
    }

    // For now, we'll just use a placeholder for function types in the symbol table.
    if (!symbolTable.define(stmt.name.lexeme, std::make_unique<IntType>(), Mutability::Immutable)) {
        error(stmt.name, "Function with this name already declared.");
        return; // Don't analyze body if redefinition.
    }

    Type* oldFunctionType = currentFunctionType;
    currentFunctionType = returnType.get();

    symbolTable.enterScope();
    for (const auto& param : stmt.params) {
        symbolTable.define(param.name.lexeme, resolveType(param.type), Mutability::Immutable);
    }

    stmt.body->accept(*this);

    symbolTable.exitScope();
    currentFunctionType = oldFunctionType;
}

void Sema::visit(const ReturnStmt& stmt) {
    if (currentFunctionType == nullptr) {
        error(stmt.keyword, "Cannot return from top-level code.");
        return;
    }

    if (stmt.value) {
        std::unique_ptr<Type> returnValType = stmt.value->accept(*this);
        if (dynamic_cast<VoidType*>(currentFunctionType)) {
            error(stmt.keyword, "Cannot return a value from a void function.");
        } else if (returnValType && !areTypesEqual(currentFunctionType, returnValType.get())) {
            error(stmt.keyword, "Return value type does not match function's return type.");
        }
    } else {
        if (!dynamic_cast<VoidType*>(currentFunctionType)) {
             error(stmt.keyword, "Must return a value from a non-void function.");
        }
    }
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
    std::unique_ptr<Type> rightType = expr.right->accept(*this);

    if (!rightType) return nullptr;

    switch (expr.op.type) {
        case TokenType::MINUS:
            if (dynamic_cast<IntType*>(rightType.get())) {
                return std::make_unique<IntType>();
            }
            error(expr.op, "Operand of '-' must be a number.");
            return nullptr;
        case TokenType::BANG:
            if (dynamic_cast<BoolType*>(rightType.get())) {
                return std::make_unique<BoolType>();
            }
            error(expr.op, "Operand of '!' must be a boolean.");
            return nullptr;
        default:
            return nullptr;
    }
}

std::unique_ptr<Type> Sema::visit(const BinaryExpr& expr) {
    if (expr.op.type == TokenType::EQUAL) {
        auto* varExpr = dynamic_cast<VariableExpr*>(expr.left.get());
        if (!varExpr) {
            error(expr.op, "Invalid assignment target.");
            return nullptr;
        }

        Symbol* symbol = symbolTable.lookup(varExpr->name.lexeme);
        if (!symbol) {
            error(varExpr->name, "Undefined variable.");
            return nullptr;
        }

        if (symbol->mutability != Mutability::Mutable) {
            error(varExpr->name, "Cannot assign to an immutable variable.");
        }

        std::unique_ptr<Type> rightType = expr.right->accept(*this);
        if (rightType && !areTypesEqual(symbol->type.get(), rightType.get())) {
            error(expr.op, "Type mismatch in assignment.");
        }
        return std::move(rightType);
    }

    std::unique_ptr<Type> leftType = expr.left->accept(*this);
    std::unique_ptr<Type> rightType = expr.right->accept(*this);

    if (!leftType || !rightType) return nullptr;

    switch (expr.op.type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
            if (dynamic_cast<IntType*>(leftType.get()) && dynamic_cast<IntType*>(rightType.get())) {
                return std::make_unique<IntType>();
            }
            error(expr.op, "Operands of binary arithmetic operators must be numbers.");
            return nullptr;

        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
             if (dynamic_cast<IntType*>(leftType.get()) && dynamic_cast<IntType*>(rightType.get())) {
                return std::make_unique<BoolType>();
            }
            error(expr.op, "Operands of comparison operators must be numbers.");
            return nullptr;

        default:
            return std::move(leftType);
    }
}

std::unique_ptr<Type> Sema::visit(const VariableExpr& expr) {
    Symbol* symbol = symbolTable.lookup(expr.name.lexeme);
    if (!symbol) {
        error(expr.name, "Undefined variable.");
        return std::make_unique<IntType>();
    }

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
