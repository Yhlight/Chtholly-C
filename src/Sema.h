#pragma once

// It's important to include headers that define types before headers that use them in templates.
// SymbolTable.h -> Symbol.h -> Type.h. This ensures Type is a complete type.
#include "SymbolTable.h"
#include "AST.h"

namespace chtholly {

class Sema : public StmtVisitor<void>, public ExprVisitor<std::unique_ptr<Type>> {
public:
    void analyze(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    void visit(const BlockStmt& stmt) override;
    void visit(const VarDeclStmt& stmt) override;

    // Other visit methods will be added later
    void visit(const ExprStmt& stmt) override {}
    void visit(const IfStmt& stmt) override {}
    void visit(const ForStmt& stmt) override {}
    void visit(const FuncDeclStmt& stmt) override {}
    void visit(const ReturnStmt& stmt) override {}

    std::unique_ptr<Type> visit(const LiteralExpr& expr) override;
    std::unique_ptr<Type> visit(const UnaryExpr& expr) override;
    std::unique_ptr<Type> visit(const BinaryExpr& expr) override;
    std::unique_ptr<Type> visit(const VariableExpr& expr) override;
    std::unique_ptr<Type> visit(const GroupingExpr& expr) override;


    SymbolTable symbolTable;
};

} // namespace chtholly
