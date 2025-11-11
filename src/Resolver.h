#ifndef CHTHOLLY_RESOLVER_H
#define CHTHOLLY_RESOLVER_H

#include "AST.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace chtholly {

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    Resolver();
    bool hadError = false;

    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    void error(const Token& token, const std::string& message);

    void resolve(const Stmt& stmt);
    void resolve(const Expr& expr);

    void beginScope();
    void endScope();
    void declare(const Token& name);
    void define(const Token& name);

    std::vector<std::map<std::string, bool>> scopes;

    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitVarStmt(const VarStmt& stmt) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt) override;
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitReturnStmt(const ReturnStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitForStmt(const ForStmt& stmt) override;
    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitCallExpr(const CallExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitGetExpr(const GetExpr& expr) override;
    std::any visitSetExpr(const SetExpr& expr) override;
    std::any visitSelfExpr(const SelfExpr& expr) override;
    std::any visitStructStmt(const StructStmt& stmt) override;

    // Default implementations for unhandled statements and expressions
    std::any visitImportStmt(const ImportStmt& stmt) override { return nullptr; }
    std::any visitSwitchStmt(const SwitchStmt& stmt) override { return nullptr; }
    std::any visitCaseStmt(const CaseStmt& stmt) override { return nullptr; }
    std::any visitBreakStmt(const BreakStmt& stmt) override { return nullptr; }
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override { return nullptr; }
    std::any visitEnumStmt(const EnumStmt& stmt) override { return nullptr; }
    std::any visitTraitStmt(const TraitStmt& stmt) override { return nullptr; }
    std::any visitLambdaExpr(const LambdaExpr& expr) override { return nullptr; }
    std::any visitBorrowExpr(const BorrowExpr& expr) override { return nullptr; }
    std::any visitDerefExpr(const DerefExpr& expr) override { return nullptr; }
    std::any visitStructLiteralExpr(const StructLiteralExpr& expr) override { return nullptr; }
    std::any visitArrayLiteralExpr(const ArrayLiteralExpr& expr) override { return nullptr; }
    std::any visitTypeCastExpr(const TypeCastExpr& expr) override { return nullptr; }
};

} // namespace chtholly

#endif // CHTHOLLY_RESOLVER_H
