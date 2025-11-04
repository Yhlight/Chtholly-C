#pragma once

#include "Expr.h"
#include "Stmt.h"
#include <string>

class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(Expr* expr);
    std::string print(Stmt* stmt);
    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitCallExpr(const CallExpr& expr) override;
    std::any visitGetExpr(const GetExpr& expr) override;
    std::any visitSetExpr(const SetExpr& expr) override;
    std::any visitBorrowExpr(const BorrowExpr& expr) override;
    std::any visitLambdaExpr(const LambdaExpr& expr) override;
    std::any visitStructInitializerExpr(const StructInitializerExpr& expr) override;

    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitLetStmt(const LetStmt& stmt) override;
    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName = std::nullopt) override;
    std::any visitReturnStmt(const ReturnStmt& stmt) override;
    std::any visitStructStmt(const StructStmt& stmt) override;
    std::any visitTraitStmt(const TraitStmt& stmt) override;
    std::any visitImplStmt(const ImplStmt& stmt) override;
    std::any visitImportStmt(const ImportStmt& stmt) override;
    std::any visitSwitchStmt(const SwitchStmt& stmt) override;
    std::any visitBreakStmt(const BreakStmt& stmt) override;
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<Expr*>& exprs);
};
