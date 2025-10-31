#pragma once

#include "AST.h"
#include "SymbolTable.h"
#include <memory>

namespace chtholly {

class Sema {
public:
    Sema();
    void analyze(const BlockStmtAST& ast);

public:
    void visit(const StmtAST& stmt);
    void visit(const VarDeclAST& stmt);
    std::shared_ptr<FunctionType> visit(const FuncDeclAST& stmt);
    void visit(const StructDeclAST& stmt);
    void visit(const TraitDeclAST& stmt);
    void visit(const IfStmtAST& stmt);
    void visit(const SwitchStmtAST& stmt);
    void visit(const CaseBlockAST& stmt);
    void visit(const EnumDeclAST& stmt);
    void visit(const WhileStmtAST& stmt);
    void visit(const ForStmtAST& stmt);
    void visit(const ImportStmtAST& stmt);
    void visit(const ExprStmtAST& stmt);
    std::shared_ptr<Type> visit(const ExprAST& expr);
    std::shared_ptr<Type> visit(const NumberExprAST& expr);
    std::shared_ptr<Type> visit(const StringExprAST& expr);
    std::shared_ptr<Type> visit(const VariableExprAST& expr);
    std::shared_ptr<Type> visit(const BinaryExprAST& expr);
    std::shared_ptr<Type> visit(const CallExprAST& expr);
    std::shared_ptr<Type> visit(const BoolExprAST& expr);
    std::shared_ptr<Type> visit(const AssignExprAST& expr);
    std::shared_ptr<Type> visit(const StructInitExprAST& expr);
    std::shared_ptr<Type> visit(const MemberAccessExprAST& expr);

    const Symbol* lookup(const std::string& name) const {
        return symbolTable.lookup(name);
    }

private:
    SymbolTable symbolTable;
};

} // namespace chtholly
