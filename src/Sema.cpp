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

void Sema::visit(const ImportStmtAST& stmt) {
    // TODO: Implement module loading and symbol importing logic.
    // For now, this is a placeholder.
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

} // namespace chtholly
