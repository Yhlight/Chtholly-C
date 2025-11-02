#ifndef CHTHOLLY_SEMA_H
#define CHTHOLLY_SEMA_H

#include "AST.h"
#include "SymbolTable.h"

namespace chtholly {

class Sema : public StmtVisitor, public ExprVisitor {
public:
    Sema();
    void analyze(const std::vector<std::unique_ptr<Stmt>>& statements);
    bool hadError() const;

    std::any visit(const AssignExpr& expr) override;
    std::any visit(const ExpressionStmt& stmt) override;
    std::any visit(const VarDeclStmt& stmt) override;
    std::any visit(const BlockStmt& stmt) override;
    std::any visit(const IfStmt& stmt) override;
    std::any visit(const WhileStmt& stmt) override;

    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GroupingExpr& expr) override;
    std::any visit(const LiteralExpr& expr) override;
    std::any visit(const UnaryExpr& expr) override;
    std::any visit(const VariableExpr& expr) override;

private:
    SymbolTable symbolTable;
    bool m_hadError;
};

} // namespace chtholly

#endif // CHTHOLLY_SEMA_H