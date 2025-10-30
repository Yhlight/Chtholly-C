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
    void visit(const ExprAST& expr);
    void visit(const NumberExprAST& expr);
    void visit(const StringExprAST& expr);
    void visit(const VariableExprAST& expr);
    void visit(const BinaryExprAST& expr);

    std::stringstream ss;
};

} // namespace chtholly
