#pragma once

#include "AST.h"
#include "SymbolTable.h"
#include <memory>

namespace chtholly {

class Sema {
public:
    Sema();
    void analyze(const BlockStmtAST& ast);

private:
    void visit(const StmtAST& stmt);
    void visit(const VarDeclAST& stmt);
    void visit(const FuncDeclAST& stmt);
    void visit(const IfStmtAST& stmt);
    void visit(const SwitchStmtAST& stmt);
    void visit(const CaseBlockAST& stmt);
    void visit(const EnumDeclAST& stmt);
    std::shared_ptr<Type> visit(const ExprAST& expr);
    std::shared_ptr<Type> visit(const NumberExprAST& expr);
    std::shared_ptr<Type> visit(const StringExprAST& expr);
    std::shared_ptr<Type> visit(const VariableExprAST& expr);
    std::shared_ptr<Type> visit(const BinaryExprAST& expr);
    std::shared_ptr<Type> visit(const CallExprAST& expr);
    std::shared_ptr<Type> visit(const BoolExprAST& expr);

    SymbolTable symbolTable;
};

} // namespace chtholly
