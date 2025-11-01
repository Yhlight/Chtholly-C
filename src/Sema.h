#pragma once

#include "SymbolTable.h"
#include "AST.h"
#include <vector>
#include <iostream>

namespace chtholly {

class Sema : public StmtVisitor<void>, public ExprVisitor<std::unique_ptr<Type>> {
public:
    Sema();
    bool hadError() const;
    void analyze(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    void visit(const BlockStmt& stmt) override;
    void visit(const VarDeclStmt& stmt) override;
    void visit(const ExprStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
    void visit(const FuncDeclStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;

    std::unique_ptr<Type> visit(const LiteralExpr& expr) override;
    std::unique_ptr<Type> visit(const UnaryExpr& expr) override;
    std::unique_ptr<Type> visit(const BinaryExpr& expr) override;
    std::unique_ptr<Type> visit(const VariableExpr& expr) override;
    std::unique_ptr<Type> visit(const GroupingExpr& expr) override;
    std::unique_ptr<Type> visit(const CallExpr& expr) override;

    std::unique_ptr<Type> resolveType(const Token& typeToken);
    void error(const Token& token, const std::string& message);

    SymbolTable symbolTable;
    Type* currentFunctionType = nullptr;
    bool errorOccurred = false;
};

} // namespace chtholly
