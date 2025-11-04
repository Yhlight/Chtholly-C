#pragma once

#include "Expr.h"
#include "Stmt.h"
#include <string>

class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(Expr* expr);
    std::string print(Stmt* stmt);
    std::string visitBinaryExpr(const BinaryExpr& expr) override;
    std::string visitGroupingExpr(const GroupingExpr& expr) override;
    std::string visitLiteralExpr(const LiteralExpr& expr) override;
    std::string visitUnaryExpr(const UnaryExpr& expr) override;
    std::string visitVariableExpr(const VariableExpr& expr) override;
    std::string visitAssignExpr(const AssignExpr& expr) override;
    std::string visitCallExpr(const CallExpr& expr) override;
    std::string visitGetExpr(const GetExpr& expr) override;
    std::string visitSetExpr(const SetExpr& expr) override;
    std::string visitBorrowExpr(const BorrowExpr& expr) override;
    std::string visitLambdaExpr(const LambdaExpr& expr) override;
    std::string visitStructInitializerExpr(const StructInitializerExpr& expr) override;

    std::string visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::string visitLetStmt(const LetStmt& stmt) override;
    std::string visitBlockStmt(const BlockStmt& stmt) override;
    std::string visitIfStmt(const IfStmt& stmt) override;
    std::string visitWhileStmt(const WhileStmt& stmt) override;
    std::string visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName = std::nullopt) override;
    std::string visitReturnStmt(const ReturnStmt& stmt) override;
    std::string visitStructStmt(const StructStmt& stmt) override;
    std::string visitTraitStmt(const TraitStmt& stmt) override;
    std::string visitImplStmt(const ImplStmt& stmt) override;
    std::string visitImportStmt(const ImportStmt& stmt) override;
    std::string visitSwitchStmt(const SwitchStmt& stmt) override;
    std::string visitBreakStmt(const BreakStmt& stmt) override;
    std::string visitFallthroughStmt(const FallthroughStmt& stmt) override;
    std::string visitEnumStmt(const EnumStmt& stmt) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<Expr*>& exprs);
};
