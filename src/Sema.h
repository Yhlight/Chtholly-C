#pragma once

#include "AST.h"
#include "SymbolTable.h"

class Sema {
public:
    Sema();

    void visit(ExprAST& ast);
    void visit(VarDeclAST& ast);
    void visit(BinaryExprAST& ast);
    void visit(VariableExprAST& ast);
    void visit(BlockExprAST& ast);
    void visit(NumberExprAST& ast);
    void visit(CallExprAST& ast);

private:
    SymbolTable symbol_table_;
};
