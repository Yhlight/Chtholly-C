#pragma once

#include "AST.h"
#include "SymbolTable.h"
#include <memory>

class Sema {
public:
    Sema();

    bool treat_int_as_move = false; // For testing purposes

    std::shared_ptr<Type> visit(ExprAST& ast);
    std::shared_ptr<Type> visit(VarDeclAST& ast);
    std::shared_ptr<Type> visit(BinaryExprAST& ast);
    std::shared_ptr<Type> visit(VariableExprAST& ast);
    std::shared_ptr<Type> visit(BlockExprAST& ast);
    std::shared_ptr<Type> visit(NumberExprAST& ast);
    std::shared_ptr<Type> visit(CallExprAST& ast);

private:
    bool is_copy_type(const std::shared_ptr<Type>& type);
    SymbolTable symbol_table_;
};
