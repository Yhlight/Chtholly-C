#include "Sema.h"
#include <stdexcept>

namespace chtholly {

Sema::Sema() {}

void Sema::analyze(const BlockStmtAST& ast) {
    for (const auto& stmt : ast.getStatements()) {
        visit(*stmt);
    }
}

void Sema::visit(const StmtAST& stmt) {
    // This is a simple dispatcher.
    // In a real compiler, you would use a more sophisticated visitor pattern.
    if (auto* varDecl = dynamic_cast<const VarDeclAST*>(&stmt)) {
        visit(*varDecl);
    } else if (auto* funcDecl = dynamic_cast<const FuncDeclAST*>(&stmt)) {
        visit(*funcDecl);
    } else if (auto* structDecl = dynamic_cast<const StructDeclAST*>(&stmt)) {
        visit(*structDecl);
    } else if (auto* traitDecl = dynamic_cast<const TraitDeclAST*>(&stmt)) {
        visit(*traitDecl);
    } else if (auto* ifStmt = dynamic_cast<const IfStmtAST*>(&stmt)) {
        visit(*ifStmt);
    } else if (auto* switchStmt = dynamic_cast<const SwitchStmtAST*>(&stmt)) {
        visit(*switchStmt);
    } else if (auto* caseBlock = dynamic_cast<const CaseBlockAST*>(&stmt)) {
        visit(*caseBlock);
    } else if (auto* enumDecl = dynamic_cast<const EnumDeclAST*>(&stmt)) {
        visit(*enumDecl);
    } else if (auto* whileStmt = dynamic_cast<const WhileStmtAST*>(&stmt)) {
        visit(*whileStmt);
    } else if (auto* forStmt = dynamic_cast<const ForStmtAST*>(&stmt)) {
        visit(*forStmt);
    } else if (auto* importStmt = dynamic_cast<const ImportStmtAST*>(&stmt)) {
        visit(*importStmt);
    } else if (auto* exprStmt = dynamic_cast<const ExprStmtAST*>(&stmt)) {
        visit(*exprStmt);
    }
}

void Sema::visit(const VarDeclAST& stmt) {
    auto initType = visit(*stmt.getInit());
    if (!symbolTable.insert(stmt.getName(), initType, stmt.getIsMutable())) {
        throw std::runtime_error("Variable already declared.");
    }
}

void Sema::visit(const FuncDeclAST& stmt) {
    std::vector<std::shared_ptr<Type>> paramTypes;
    for (const auto& param : stmt.getParams()) {
        // For now, we will just create a placeholder type.
        // In a real compiler, you would resolve the type name to a Type object.
        if (param.typeName == "int") {
            paramTypes.push_back(std::make_shared<IntType>());
        } else if (param.typeName == "double") {
            paramTypes.push_back(std::make_shared<DoubleType>());
        } else if (param.typeName == "string") {
            paramTypes.push_back(std::make_shared<StringType>());
        } else {
            throw std::runtime_error("Unknown type: " + param.typeName);
        }
    }

    std::shared_ptr<Type> returnType;
    if (stmt.getReturnTypeName() == "int") {
        returnType = std::make_shared<IntType>();
    } else if (stmt.getReturnTypeName() == "double") {
        returnType = std::make_shared<DoubleType>();
    } else if (stmt.getReturnTypeName() == "string") {
        returnType = std::make_shared<StringType>();
    } else if (stmt.getReturnTypeName() == "void") {
        returnType = std::make_shared<VoidType>();
    } else {
        throw std::runtime_error("Unknown type: " + stmt.getReturnTypeName());
    }

    auto funcType = std::make_shared<FunctionType>(returnType, paramTypes);
    if (!symbolTable.insert(stmt.getName(), funcType, false)) {
        throw std::runtime_error("Function already declared.");
    }

    symbolTable.enterScope();
    for (size_t i = 0; i < stmt.getParams().size(); ++i) {
        if (!symbolTable.insert(stmt.getParams()[i].name, paramTypes[i], false)) {
            throw std::runtime_error("Parameter already declared.");
        }
    }
    analyze(stmt.getBody());
    symbolTable.exitScope();
}

void Sema::visit(const StructDeclAST& stmt) {
    std::vector<MemberVarType> memberTypes;
    for (const auto& member : stmt.getMembers()) {
        std::shared_ptr<Type> memberType;
        if (member.typeName == "int") {
            memberType = std::make_shared<IntType>();
        } else if (member.typeName == "double") {
            memberType = std::make_shared<DoubleType>();
        } else if (member.typeName == "string") {
            memberType = std::make_shared<StringType>();
        } else {
            auto symbol = symbolTable.lookup(member.typeName);
            if (!symbol) {
                throw std::runtime_error("Unknown type: " + member.typeName);
            }
            memberType = symbol->type;
        }
        memberTypes.push_back({member.name, memberType, member.isPublic});
    }

    auto structType = std::make_shared<StructType>(stmt.getName(), memberTypes);
    if (!symbolTable.insert(stmt.getName(), structType, false)) {
        throw std::runtime_error("Struct already declared.");
    }
}

void Sema::visit(const TraitDeclAST& stmt) {
    std::vector<std::shared_ptr<FunctionType>> methodTypes;
    for (const auto& method : stmt.getMethods()) {
        std::vector<std::shared_ptr<Type>> paramTypes;
        for (const auto& param : method->getParams()) {
            if (param.typeName == "int") {
                paramTypes.push_back(std::make_shared<IntType>());
            } else if (param.typeName == "double") {
                paramTypes.push_back(std::make_shared<DoubleType>());
            } else if (param.typeName == "string") {
                paramTypes.push_back(std::make_shared<StringType>());
            } else {
                auto symbol = symbolTable.lookup(param.typeName);
                if (!symbol) {
                    throw std::runtime_error("Unknown type: " + param.typeName);
                }
                paramTypes.push_back(symbol->type);
            }
        }

        std::shared_ptr<Type> returnType;
        if (method->getReturnTypeName() == "int") {
            returnType = std::make_shared<IntType>();
        } else if (method->getReturnTypeName() == "double") {
            returnType = std::make_shared<DoubleType>();
        } else if (method->getReturnTypeName() == "string") {
            returnType = std::make_shared<StringType>();
        } else if (method->getReturnTypeName() == "void") {
            returnType = std::make_shared<VoidType>();
        } else {
            auto symbol = symbolTable.lookup(method->getReturnTypeName());
            if (!symbol) {
                throw std::runtime_error("Unknown type: " + method->getReturnTypeName());
            }
            returnType = symbol->type;
        }

        methodTypes.push_back(std::make_shared<FunctionType>(returnType, paramTypes));
    }

    auto traitType = std::make_shared<TraitType>(stmt.getName(), methodTypes);
    if (!symbolTable.insert(stmt.getName(), traitType, false)) {
        throw std::runtime_error("Trait already declared.");
    }
}

void Sema::visit(const IfStmtAST& stmt) {
    auto condType = visit(stmt.getCond());
    if (condType->getKind() != TypeKind::Bool) {
        throw std::runtime_error("If condition must be a boolean expression.");
    }

    symbolTable.enterScope();
    analyze(stmt.getThen());
    symbolTable.exitScope();

    if (stmt.getElse()) {
        symbolTable.enterScope();
        analyze(*stmt.getElse());
        symbolTable.exitScope();
    }
}

void Sema::visit(const SwitchStmtAST& stmt) {
    auto exprType = visit(stmt.getExpr());
    for (const auto& caseBlock : stmt.getCases()) {
        if (caseBlock->getExpr()) {
            auto caseType = visit(*caseBlock->getExpr());
            if (exprType->getKind() != caseType->getKind()) {
                throw std::runtime_error("Case expression type does not match switch expression type.");
            }
        }
        visit(*caseBlock);
    }
}

void Sema::visit(const CaseBlockAST& stmt) {
    symbolTable.enterScope();
    analyze(stmt.getBody());
    symbolTable.exitScope();
}

void Sema::visit(const EnumDeclAST& stmt) {
    auto enumType = std::make_shared<EnumType>(stmt.getName(), stmt.getMembers());
    if (!symbolTable.insert(stmt.getName(), enumType, false)) {
        throw std::runtime_error("Enum already declared.");
    }

    for (const auto& member : stmt.getMembers()) {
        if (!symbolTable.insert(stmt.getName() + "::" + member, enumType, false)) {
            throw std::runtime_error("Enum member already declared.");
        }
    }
}

void Sema::visit(const WhileStmtAST& stmt) {
    auto condType = visit(stmt.getCond());
    if (condType->getKind() != TypeKind::Bool) {
        throw std::runtime_error("While condition must be a boolean expression.");
    }

    symbolTable.enterScope();
    analyze(stmt.getBody());
    symbolTable.exitScope();
}

void Sema::visit(const ForStmtAST& stmt) {
    symbolTable.enterScope();

    if (stmt.getInit()) {
        visit(*stmt.getInit());
    }

    if (stmt.getCond()) {
        auto condType = visit(*stmt.getCond());
        if (condType->getKind() != TypeKind::Bool) {
            throw std::runtime_error("For loop condition must be a boolean expression.");
        }
    }

    if (stmt.getInc()) {
        visit(*stmt.getInc());
    }

    analyze(stmt.getBody());

    symbolTable.exitScope();
}

void Sema::visit(const ExprStmtAST& stmt) {
    visit(stmt.getExpr());
}

void Sema::visit(const ImportStmtAST& stmt) {
    if (stmt.getModuleName() == "iostream") {
        // Manually add the 'print' function symbol to the symbol table.
        auto stringType = std::make_shared<StringType>();
        auto voidType = std::make_shared<VoidType>();
        std::vector<std::shared_ptr<Type>> paramTypes = {stringType};
        auto printFuncType = std::make_shared<FunctionType>(voidType, paramTypes);
        if (!symbolTable.insert("print", printFuncType, false)) {
            // This could happen if iostream is imported multiple times.
            // For now, we'll just ignore it.
        }
    }
}

std::shared_ptr<Type> Sema::visit(const ExprAST& expr) {
    // This is a simple dispatcher.
    if (auto* numExpr = dynamic_cast<const NumberExprAST*>(&expr)) {
        return visit(*numExpr);
    }
    if (auto* stringExpr = dynamic_cast<const StringExprAST*>(&expr)) {
        return visit(*stringExpr);
    }
    if (auto* varExpr = dynamic_cast<const VariableExprAST*>(&expr)) {
        return visit(*varExpr);
    }
    if (auto* binExpr = dynamic_cast<const BinaryExprAST*>(&expr)) {
        return visit(*binExpr);
    }
    if (auto* callExpr = dynamic_cast<const CallExprAST*>(&expr)) {
        return visit(*callExpr);
    }
    if (auto* boolExpr = dynamic_cast<const BoolExprAST*>(&expr)) {
        return visit(*boolExpr);
    }
    if (auto* assignExpr = dynamic_cast<const AssignExprAST*>(&expr)) {
        return visit(*assignExpr);
    }
    if (auto* structInitExpr = dynamic_cast<const StructInitExprAST*>(&expr)) {
        return visit(*structInitExpr);
    }
    if (auto* memberAccessExpr = dynamic_cast<const MemberAccessExprAST*>(&expr)) {
        return visit(*memberAccessExpr);
    }
    return nullptr;
}

std::shared_ptr<Type> Sema::visit(const AssignExprAST& expr) {
    auto targetType = visit(expr.getTarget());
    auto valueType = visit(expr.getValue());

    if (auto* varExpr = dynamic_cast<const VariableExprAST*>(&expr.getTarget())) {
        auto symbol = symbolTable.lookup(varExpr->getName());
        if (!symbol) {
            throw std::runtime_error("Variable not declared.");
        }
        if (!symbol->isMutable) {
            throw std::runtime_error("Cannot assign to an immutable variable.");
        }
    } else {
        throw std::runtime_error("Invalid assignment target.");
    }

    if (targetType->getKind() != valueType->getKind()) {
        throw std::runtime_error("Type mismatch in assignment.");
    }
    return valueType;
}

std::shared_ptr<Type> Sema::visit(const NumberExprAST& expr) {
    if (std::holds_alternative<int>(expr.getVal())) {
        return std::make_shared<IntType>();
    } else if (std::holds_alternative<double>(expr.getVal())) {
        return std::make_shared<DoubleType>();
    }
    return nullptr;
}

std::shared_ptr<Type> Sema::visit(const StringExprAST& expr) {
    return std::make_shared<StringType>();
}

std::shared_ptr<Type> Sema::visit(const VariableExprAST& expr) {
    auto symbol = symbolTable.lookup(expr.getName());
    if (!symbol) {
        throw std::runtime_error("Variable not declared.");
    }
    return symbol->type;
}

std::shared_ptr<Type> Sema::visit(const BinaryExprAST& expr) {
    auto lhsType = visit(expr.getLHS());
    auto rhsType = visit(expr.getRHS());
    // This is a simplified version.
    // In a real compiler, you would check for compatible types.
    if (lhsType->getKind() != rhsType->getKind()) {
        throw std::runtime_error("Type mismatch in binary expression.");
    }
    return lhsType;
}

std::shared_ptr<Type> Sema::visit(const CallExprAST& expr) {
    if (auto* varExpr = dynamic_cast<const VariableExprAST*>(&expr.getCallee())) {
        auto symbol = symbolTable.lookup(varExpr->getName());
        if (symbol && symbol->type->getKind() == TypeKind::Struct) {
            if (!expr.getArgs().empty()) {
                throw std::runtime_error("Struct default instantiation does not take arguments.");
            }
            return symbol->type;
        }
    }

    auto calleeType = visit(expr.getCallee());
    if (calleeType->getKind() != TypeKind::Function) {
        throw std::runtime_error("Cannot call a non-function.");
    }

    auto funcType = std::static_pointer_cast<FunctionType>(calleeType);
    const auto& paramTypes = funcType->getParamTypes();
    const auto& args = expr.getArgs();

    if (paramTypes.size() != args.size()) {
        throw std::runtime_error("Incorrect number of arguments.");
    }

    for (size_t i = 0; i < args.size(); ++i) {
        auto argType = visit(*args[i]);
        if (argType->getKind() != paramTypes[i]->getKind()) {
            throw std::runtime_error("Incorrect argument type.");
        }
    }

    return funcType->getReturnType();
}

std::shared_ptr<Type> Sema::visit(const BoolExprAST& expr) {
    return std::make_shared<BoolType>();
}

std::shared_ptr<Type> Sema::visit(const StructInitExprAST& expr) {
    auto symbol = symbolTable.lookup(expr.getStructName());
    if (!symbol || symbol->type->getKind() != TypeKind::Struct) {
        throw std::runtime_error("Unknown struct type: " + expr.getStructName());
    }

    auto structType = std::static_pointer_cast<StructType>(symbol->type);
    const auto& structMembers = structType->getMembers();
    const auto& initMembers = expr.getMembers();

    if (structMembers.size() != initMembers.size()) {
        throw std::runtime_error("Incorrect number of members in struct initializer.");
    }

    for (size_t i = 0; i < initMembers.size(); ++i) {
        const auto& initMember = initMembers[i];
        const auto& structMember = structMembers[i];

        if (initMember.name != structMember.name) {
            throw std::runtime_error("Member name mismatch in struct initializer.");
        }

        auto initType = visit(*initMember.value);
        if (initType->getKind() != structMember.type->getKind()) {
            throw std::runtime_error("Type mismatch for member '" + initMember.name + "'.");
        }
    }

    return structType;
}

std::shared_ptr<Type> Sema::visit(const MemberAccessExprAST& expr) {
    auto objectType = visit(expr.getObject());
    if (objectType->getKind() != TypeKind::Struct) {
        throw std::runtime_error("Member access on non-struct type.");
    }

    auto structType = std::static_pointer_cast<StructType>(objectType);
    for (const auto& member : structType->getMembers()) {
        if (member.name == expr.getMemberName()) {
            if (!member.isPublic) {
                throw std::runtime_error("Cannot access private member '" + expr.getMemberName() + "'.");
            }
            return member.type;
        }
    }

    throw std::runtime_error("Struct '" + structType->getName() + "' has no member '" + expr.getMemberName() + "'.");
}

} // namespace chtholly
