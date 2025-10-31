#pragma once

#include "AST.h"
#include <string>
#include <sstream>

namespace chtholly {

class CodeGen {
public:
    CodeGen();
    std::string generate(const BlockStmtAST& ast);

private:
    void visit(const StmtAST& stmt);
    void visit(const VarDeclAST& stmt);
    void visit(const FuncDeclAST& stmt);
    void visit(const IfStmtAST& stmt);
    void visit(const SwitchStmtAST& stmt);
    void visit(const CaseBlockAST& stmt);
    void visit(const EnumDeclAST& stmt);
    void visit(const WhileStmtAST& stmt);
    void visit(const ForStmtAST& stmt);
    void visit(const ImportStmtAST& stmt);
    void visit(const ExprStmtAST& stmt);
    void visit(const BlockStmtAST& stmt);
    void visit(const ExprAST& expr);
    void visit(const NumberExprAST& expr);
    void visit(const StringExprAST& expr);
    void visit(const VariableExprAST& expr);
    void visit(const BinaryExprAST& expr);
    void visit(const CallExprAST& expr);
    void visit(const BoolExprAST& expr);
    void visit(const AssignExprAST& expr);

    std::stringstream ss;
    bool inForInit = false;
};

} // namespace chtholly
