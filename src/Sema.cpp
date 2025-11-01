#include "Sema.h"
#include <iostream>

namespace chtholly {

// Helper to check if two types are the same.
bool areTypesEqual(const Type* a, const Type* b) {
    if (!a || !b) return a == b;

    if (typeid(*a) != typeid(*b)) return false;

    if (dynamic_cast<const IntType*>(a)) return true;
    if (dynamic_cast<const StringType*>(a)) return true;
    if (dynamic_cast<const BoolType*>(a)) return true;
    if (dynamic_cast<const VoidType*>(a)) return true;

    if (auto* enumA = dynamic_cast<const EnumType*>(a)) {
        auto* enumB = dynamic_cast<const EnumType*>(b);
        return enumA->getName() == enumB->getName();
    }

    if (auto* arrayA = dynamic_cast<const ArrayType*>(a)) {
        auto* arrayB = dynamic_cast<const ArrayType*>(b);
        if (arrayA->getElementType() == nullptr || arrayB->getElementType() == nullptr) {
            return true;
        }
        return areTypesEqual(arrayA->getElementType(), arrayB->getElementType());
    }

    if (auto* refA = dynamic_cast<const ReferenceType*>(a)) {
        auto* refB = dynamic_cast<const ReferenceType*>(b);
        return refA->getIsMutable() == refB->getIsMutable() &&
               areTypesEqual(refA->getReferencedType(), refB->getReferencedType());
    }

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
    if (stmt.type_expr) {
        declaredType = resolveType(*stmt.type_expr);
    }

    std::unique_ptr<Type> initializerType = nullptr;
    if (stmt.initializer) {
        initializerType = stmt.initializer->accept(*this);

        if (auto* varExpr = dynamic_cast<VariableExpr*>(stmt.initializer.get())) {
            Symbol* symbol = symbolTable.lookup(varExpr->name.lexeme);
            if (symbol && !symbol->type->isCopyable()) {
                symbol->state = OwnershipState::Moved;
                varExpr->isMove = true;
            }
        }
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

    stmt.resolvedType = finalType->clone();
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
    std::vector<std::unique_ptr<Type>> paramTypes;
    for (const auto& param : stmt.params) {
        paramTypes.push_back(resolveType(param.type));
    }

    std::unique_ptr<Type> returnType = std::make_unique<VoidType>();
    if (stmt.returnType) {
        returnType = resolveType(*stmt.returnType);
    }

    auto funcType = std::make_unique<FunctionType>(std::move(paramTypes), std::move(returnType));

    // We need to cast the result of clone() from unique_ptr<Type> to unique_ptr<FunctionType>
    auto cloned_type = funcType->clone();
    stmt.resolvedSignature.reset(dynamic_cast<FunctionType*>(cloned_type.release()));

    if (!symbolTable.define(stmt.name.lexeme, std::move(funcType), Mutability::Immutable)) {
        error(stmt.name, "Function with this name already declared.");
        return; // Don't analyze body if redefinition.
    }

    Type* oldFunctionType = currentFunctionType;
    currentFunctionType = symbolTable.lookup(stmt.name.lexeme)->type.get();

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

    auto* funcType = dynamic_cast<FunctionType*>(currentFunctionType);
    if (!funcType) return;

    if (stmt.value) {
        std::unique_ptr<Type> returnValType = stmt.value->accept(*this);

        if (auto* varExpr = dynamic_cast<VariableExpr*>(stmt.value.get())) {
            Symbol* symbol = symbolTable.lookup(varExpr->name.lexeme);
            if (symbol && !symbol->type->isCopyable()) {
                symbol->state = OwnershipState::Moved;
                varExpr->isMove = true;
            }
        }

        if (dynamic_cast<const VoidType*>(funcType->getReturnType())) {
            error(stmt.keyword, "Cannot return a value from a void function.");
        } else if (returnValType && !areTypesEqual(funcType->getReturnType(), returnValType.get())) {
            error(stmt.keyword, "Return value type does not match function's return type.");
        }
    } else {
        if (!dynamic_cast<const VoidType*>(funcType->getReturnType())) {
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
    // Handle assignment separately to avoid incorrect "use of moved value" error.
    if (expr.op.type == TokenType::EQUAL || expr.op.type == TokenType::PLUS_EQUAL ||
        expr.op.type == TokenType::MINUS_EQUAL || expr.op.type == TokenType::STAR_EQUAL ||
        expr.op.type == TokenType::SLASH_EQUAL || expr.op.type == TokenType::PERCENT_EQUAL) {

        // First, visit the right-hand side to check for moved values there.
        std::unique_ptr<Type> rightType = expr.right->accept(*this);
        if (!rightType) return nullptr;

        std::unique_ptr<Type> leftType;

        // Now, handle the left-hand side without visiting the VariableExpr,
        // which would trigger the ownership check.
        if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.left.get())) {
            Symbol* symbol = symbolTable.lookup(varExpr->name.lexeme);
            if (!symbol) {
                error(varExpr->name, "Undefined variable.");
                return nullptr;
            }
            if (symbol->mutability != Mutability::Mutable) {
                error(varExpr->name, "Cannot assign to an immutable variable.");
            }
            // This is the key: assignment restores ownership, so we set it to Valid.
            symbol->state = OwnershipState::Valid;
            leftType = symbol->type->clone();
        } else if (auto* derefExpr = dynamic_cast<DerefExpr*>(expr.left.get())) {
            // For dereferences, we visit the inner expression which should be a reference.
            std::unique_ptr<Type> refType = derefExpr->expression->accept(*this);
            auto* referenceType = dynamic_cast<ReferenceType*>(refType.get());
            if (!referenceType || !referenceType->getIsMutable()) {
                error(expr.op, "Cannot assign to an immutable reference.");
            }
            if (referenceType) {
                leftType = referenceType->getReferencedType()->clone();
            }
        } else {
            error(expr.op, "Invalid assignment target.");
            return nullptr;
        }

        if (!leftType) return nullptr;

        // Finally, check for type mismatch.
        if (!areTypesEqual(leftType.get(), rightType.get())) {
            error(expr.op, "Type mismatch in assignment.");
        }
        return std::move(rightType);
    }

    // Standard binary expressions
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
        case TokenType::BANG_EQUAL:
        case TokenType::EQUAL_EQUAL:
             if (dynamic_cast<IntType*>(leftType.get()) && dynamic_cast<IntType*>(rightType.get())) {
                return std::make_unique<BoolType>();
            }
            error(expr.op, "Operands of comparison operators must be numbers.");
            return nullptr;

        case TokenType::AND:
        case TokenType::OR:
            if (dynamic_cast<BoolType*>(leftType.get()) && dynamic_cast<BoolType*>(rightType.get())) {
                return std::make_unique<BoolType>();
            }
            error(expr.op, "Operands of logical operators must be booleans.");
            return nullptr;

        default:
            return std::move(leftType);
    }
}

std::unique_ptr<Type> Sema::visit(const VariableExpr& expr) {
    Symbol* symbol = symbolTable.lookup(expr.name.lexeme);
    if (!symbol) {
        error(expr.name, "Undefined variable.");
        return nullptr;
    }

    if (symbol->state == OwnershipState::Moved) {
        error(expr.name, "Use of moved value.");
        // We still return the type to avoid cascading errors about "undefined variable".
        return symbol->type->clone();
    }

    // Don't "evaluate" a function variable to a type here. Let CallExpr handle it.
    if (dynamic_cast<FunctionType*>(symbol->type.get())) {
        return nullptr;
    }

    return symbol->type->clone();
}

std::unique_ptr<Type> Sema::visit(const GroupingExpr& expr) {
    return expr.expression->accept(*this);
}

std::unique_ptr<Type> Sema::visit(const CallExpr& expr) {
    auto* varExpr = dynamic_cast<VariableExpr*>(expr.callee.get());
    if (!varExpr) {
        error(expr.getToken(), "Can only call functions by name.");
        return nullptr;
    }

    Symbol* symbol = symbolTable.lookup(varExpr->name.lexeme);
    if (!symbol) {
        error(varExpr->name, "Undefined function.");
        return nullptr;
    }

    auto* funcType = dynamic_cast<FunctionType*>(symbol->type.get());
    if (!funcType) {
        error(expr.getToken(), "Can only call functions and methods.");
        return nullptr;
    }

    if (expr.arguments.size() != funcType->getParamTypes().size()) {
        error(expr.getToken(), "Expected " + std::to_string(funcType->getParamTypes().size()) + " arguments but got " + std::to_string(expr.arguments.size()) + ".");
        return nullptr;
    }

    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        std::unique_ptr<Type> argType = expr.arguments[i]->accept(*this);
        if (argType && !areTypesEqual(funcType->getParamTypes()[i].get(), argType.get())) {
            error(expr.arguments[i]->getToken(), "Argument type mismatch.");
        }

        // Check for moves on non-copyable types
        if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.arguments[i].get())) {
            Symbol* argSymbol = symbolTable.lookup(varExpr->name.lexeme);
            if (argSymbol && !argSymbol->type->isCopyable()) {
                argSymbol->state = OwnershipState::Moved;
                varExpr->isMove = true;
            }
        }
    }

    // This is not a proper clone, but good enough for now.
    if (dynamic_cast<const IntType*>(funcType->getReturnType())) return std::make_unique<IntType>();
    if (dynamic_cast<const StringType*>(funcType->getReturnType())) return std::make_unique<StringType>();
    if (dynamic_cast<const BoolType*>(funcType->getReturnType())) return std::make_unique<BoolType>();
    if (dynamic_cast<const VoidType*>(funcType->getReturnType())) return std::make_unique<VoidType>();

    return nullptr;
}

std::unique_ptr<Type> Sema::visit(const LambdaExpr& expr) {
    std::vector<std::unique_ptr<Type>> paramTypes;
    for (const auto& param : expr.params) {
        paramTypes.push_back(resolveType(param.type));
    }

    std::unique_ptr<Type> returnType = std::make_unique<VoidType>();
    if (expr.returnType) {
        returnType = resolveType(*expr.returnType);
    }

    auto funcType = std::make_unique<FunctionType>(std::move(paramTypes), std::move(returnType));

    // We need to clone the function type because the original will be moved into the symbol table
    // (if we were to give lambdas names), but we need to return it from this visitor.
    // For now, since lambdas are anonymous, we just analyze the body and return the type.

    Type* oldFunctionType = currentFunctionType;
    currentFunctionType = funcType.get(); // Set the current function type for return statement checking

    symbolTable.enterScope();
    for (const auto& param : expr.params) {
        symbolTable.define(param.name.lexeme, resolveType(param.type), Mutability::Immutable);
    }

    expr.body->accept(*this);

    symbolTable.exitScope();
    currentFunctionType = oldFunctionType;

    return funcType;
}

void Sema::visit(const SwitchStmt& stmt) {
    std::unique_ptr<Type> conditionType = stmt.condition->accept(*this);
    if (!conditionType) return;

    bool oldInSwitch = inSwitch;
    inSwitch = true;

    for (size_t i = 0; i < stmt.cases.size(); ++i) {
        const auto& caseStmt = stmt.cases[i];
        if (caseStmt->condition) {
            std::unique_ptr<Type> caseConditionType = caseStmt->condition->accept(*this);
            if (caseConditionType && !areTypesEqual(conditionType.get(), caseConditionType.get())) {
                error(caseStmt->condition->getToken(), "Case condition type does not match switch condition type.");
            }
        }
        caseStmt->body->accept(*this);

        if (i == stmt.cases.size() - 1) {
            if (auto* block = dynamic_cast<BlockStmt*>(caseStmt->body.get())) {
                if (!block->statements.empty() && block->statements.back()->isFallthrough()) {
                    error(dynamic_cast<FallthroughStmt*>(block->statements.back().get())->keyword, "Cannot use 'fallthrough' in the last case of a switch statement.");
                }
            }
        }
    }

    inSwitch = oldInSwitch;
}

void Sema::visit(const CaseStmt& stmt) {
    // This is handled by visit(SwitchStmt&)
}

void Sema::visit(const BreakStmt& stmt) {
    if (!inSwitch) {
        error(stmt.keyword, "Cannot use 'break' outside of a switch statement or loop.");
    }
}

void Sema::visit(const FallthroughStmt& stmt) {
    if (!inSwitch) {
        error(stmt.keyword, "Cannot use 'fallthrough' outside of a switch statement.");
    }
    // A simple check for being the last statement in a block can be tricky.
    // For now, we will rely on a runtime check. A more advanced static analysis
    // would be needed to correctly identify the last case.
}

std::unique_ptr<Type> Sema::visit(const StructInitExpr& expr) {
    Symbol* symbol = symbolTable.lookup(expr.name.lexeme);
    if (!symbol) {
        error(expr.name, "Struct with this name not found.");
        return nullptr;
    }

    auto* structType = dynamic_cast<StructType*>(symbol->type.get());
    if (!structType) {
        error(expr.name, "Identifier is not a struct type.");
        return nullptr;
    }

    if (expr.members.size() != structType->getMembers().size()) {
        error(expr.name, "Incorrect number of members in struct initializer.");
        return nullptr;
    }

    for (const auto& initMember : expr.members) {
        bool found = false;
        for (const auto& structMember : structType->getMembers()) {
            if (initMember.name.lexeme == structMember.name) {
                found = true;
                std::unique_ptr<Type> initType = initMember.initializer->accept(*this);
                if (initType && !areTypesEqual(structMember.type.get(), initType.get())) {
                    error(initMember.name, "Initializer type does not match member type.");
                }
                break;
            }
        }
        if (!found) {
            error(initMember.name, "Member with this name not found in struct.");
        }
    }

    return structType->clone();
}

std::unique_ptr<Type> Sema::visit(const MemberAccessExpr& expr) {
    std::unique_ptr<Type> objectType;
    if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.object.get())) {
        Symbol* symbol = symbolTable.lookup(varExpr->name.lexeme);
        if (!symbol) {
            error(varExpr->name, "Undefined variable.");
            return nullptr;
        }
        objectType = symbol->type->clone();
    } else {
        objectType = expr.object->accept(*this);
    }

    if (!objectType) return nullptr;

    auto* structType = dynamic_cast<StructType*>(objectType.get());
    if (!structType) {
        error(expr.name, "Can only access members of a struct.");
        return nullptr;
    }

    for (const auto& member : structType->getMembers()) {
        if (expr.name.lexeme == member.name) {
            return member.type->clone();
        }
    }

    error(expr.name, "Member with this name not found in struct.");
    return nullptr;
}

void Sema::visit(const StructDeclStmt& stmt) {
    std::vector<StructType::Member> members;
    for (const auto& member : stmt.members) {
        members.push_back({member.name.lexeme, resolveType(member.type)});
    }

    auto structType = std::make_unique<StructType>(stmt.name.lexeme, std::move(members));

    if (!symbolTable.define(stmt.name.lexeme, std::move(structType), Mutability::Immutable)) {
        error(stmt.name, "Struct with this name already declared in this scope.");
    }
}

void Sema::visit(const EnumDeclStmt& stmt) {
    std::vector<std::string> memberNames;
    for (const auto& member : stmt.members) {
        memberNames.push_back(member.lexeme);
    }

    auto enumType = std::make_unique<EnumType>(stmt.name.lexeme, std::move(memberNames));

    if (!symbolTable.define(stmt.name.lexeme, std::move(enumType), Mutability::Immutable)) {
        error(stmt.name, "Enum with this name already declared in this scope.");
    }
}

std::unique_ptr<Type> Sema::visit(const ScopedAccessExpr& expr) {
    auto* varExpr = dynamic_cast<VariableExpr*>(expr.scope.get());
    if (!varExpr) {
        error(expr.getToken(), "Invalid scope resolution target.");
        return nullptr;
    }

    Symbol* symbol = symbolTable.lookup(varExpr->name.lexeme);
    if (!symbol) {
        error(varExpr->name, "Undefined variable.");
        return nullptr;
    }

    auto* enumType = dynamic_cast<EnumType*>(symbol->type.get());
    if (!enumType) {
        error(expr.getToken(), "Can only access members of an enum.");
        return nullptr;
    }

    if (!enumType->hasMember(expr.name.lexeme)) {
        error(expr.name, "Member with this name not found in enum.");
        return nullptr;
    }

    return enumType->clone();
}

std::unique_ptr<Type> Sema::visit(const ArrayLiteralExpr& expr) {
    if (expr.elements.empty()) {
        // Cannot determine the type of an empty array literal without context.
        // This should be resolved by the variable declaration's type annotation.
        return std::make_unique<ArrayType>(nullptr);
    }

    std::unique_ptr<Type> firstElementType = expr.elements[0]->accept(*this);
    if (!firstElementType) return nullptr;

    for (size_t i = 1; i < expr.elements.size(); ++i) {
        std::unique_ptr<Type> elementType = expr.elements[i]->accept(*this);
        if (elementType && !areTypesEqual(firstElementType.get(), elementType.get())) {
            error(expr.elements[i]->getToken(), "Array elements must have the same type.");
            return nullptr;
        }
    }

    return std::make_unique<ArrayType>(std::move(firstElementType));
}

std::unique_ptr<Type> Sema::visit(const SubscriptExpr& expr) {
    std::unique_ptr<Type> arrayType = expr.array->accept(*this);
    if (!arrayType) return nullptr;

    auto* arrType = dynamic_cast<ArrayType*>(arrayType.get());
    if (!arrType) {
        error(expr.bracket, "Can only subscript an array.");
        return nullptr;
    }

    std::unique_ptr<Type> indexType = expr.index->accept(*this);
    if (indexType && !dynamic_cast<IntType*>(indexType.get())) {
        error(expr.index->getToken(), "Array index must be an integer.");
        return nullptr;
    }

    return arrType->getElementType()->clone();
}

std::unique_ptr<Type> Sema::visit(const BorrowExpr& expr) {
    std::unique_ptr<Type> referencedType = expr.expression->accept(*this);
    if (!referencedType) return nullptr;

    if (expr.isMutable) {
        if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.expression.get())) {
            Symbol* symbol = symbolTable.lookup(varExpr->name.lexeme);
            if (symbol && symbol->mutability != Mutability::Mutable) {
                error(expr.ampersand, "Cannot mutably borrow an immutable variable.");
            }
        }
    }

    return std::make_unique<ReferenceType>(std::move(referencedType), expr.isMutable);
}

std::unique_ptr<Type> Sema::visit(const DerefExpr& expr) {
    std::unique_ptr<Type> refType = expr.expression->accept(*this);
    if (!refType) return nullptr;

    auto* referenceType = dynamic_cast<ReferenceType*>(refType.get());
    if (!referenceType) {
        error(expr.star, "Cannot dereference a non-reference type.");
        return nullptr;
    }

    return referenceType->getReferencedType()->clone();
}


std::unique_ptr<Type> Sema::resolveType(const Token& typeToken) {
    if (typeToken.lexeme == "int") return std::make_unique<IntType>();
    if (typeToken.lexeme == "string") return std::make_unique<StringType>();
    if (typeToken.lexeme == "bool") return std::make_unique<BoolType>();
    if (typeToken.lexeme == "void") return std::make_unique<VoidType>();

    Symbol* symbol = symbolTable.lookup(typeToken.lexeme);
    if (symbol && (dynamic_cast<StructType*>(symbol->type.get()) || dynamic_cast<EnumType*>(symbol->type.get()))) {
        return symbol->type->clone();
    }

    error(typeToken, "Unknown type name.");
    return nullptr;
}

std::unique_ptr<Type> Sema::resolveType(const TypeExpr& typeExpr) {
    if (auto* simple = dynamic_cast<const SimpleTypeExpr*>(&typeExpr)) {
        return resolveType(simple->name);
    }
    if (auto* array = dynamic_cast<const ArrayTypeExpr*>(&typeExpr)) {
        std::unique_ptr<Type> elementType = resolveType(*array->element_type);
        return std::make_unique<ArrayType>(std::move(elementType), array->size);
    }
    if (auto* ref = dynamic_cast<const ReferenceTypeExpr*>(&typeExpr)) {
        std::unique_ptr<Type> referencedType = resolveType(*ref->referenced_type);
        // For now, we assume all references in type annotations are immutable.
        // Mutable borrows are only introduced by `&mut` expressions.
        return std::make_unique<ReferenceType>(std::move(referencedType), false);
    }
    return nullptr;
}


void Sema::error(const Token& token, const std::string& message) {
    errorOccurred = true;
    std::cerr << "Error at '" << token.lexeme << "' (line " << token.line << "): " << message << std::endl;
}

} // namespace chtholly
