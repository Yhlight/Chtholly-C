#ifndef CHTHOLLY_RESOLVER_H
#define CHTHOLLY_RESOLVER_H

#include <vector>
#include <memory>
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "SymbolTable.h"

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    Resolver();
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    void resolve(Stmt* stmt);
    void resolve(Expr* expr);

    void visitBlockStmt(const Block& stmt) override;
    void visitExpressionStmt(const ExpressionStmt& stmt) override;
    void visitFuncStmt(const FuncStmt& stmt) override;
    void visitLetStmt(const LetStmt& stmt) override;
    void visitPrintStmt(const PrintStmt& stmt) override;
    void visitReturnStmt(const ReturnStmt& stmt) override;

    void visitAssignExpr(const Assign& expr) override;
    void visitBinaryExpr(const Binary& expr) override;
    void visitCallExpr(const Call& expr) override;
    void visitGroupingExpr(const Grouping& expr) override;
    void visitLiteralExpr(const Literal& expr) override;
    void visitUnaryExpr(const Unary& expr) override;
    void visitVariableExpr(const Variable& expr) override;

    SymbolTable symbolTable_;
};

#endif // CHTHOLLY_RESOLVER_H
