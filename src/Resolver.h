#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <map>
#include <string>

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;

    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitPrintStmt(const PrintStmt& stmt) override;
    std::any visitLetStmt(const LetStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitSwitchStmt(const SwitchStmt& stmt) override;
    std::any visitCaseStmt(const CaseStmt& stmt) override;
    std::any visitBreakStmt(const BreakStmt& stmt) override;
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override;

private:
    void resolve(const Stmt& stmt);
    void resolve(const Expr& expr);

    void beginScope();
    void endScope();

    void declare(const Token& name);
    void define(const Token& name);

    std::vector<std::map<std::string, bool>> scopes;
    bool inSwitch = false;
};
