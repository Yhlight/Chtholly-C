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
    return nullptr;
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

} // namespace chtholly
