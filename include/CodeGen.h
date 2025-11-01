#ifndef CHTHOLLY_CODEGEN_H
#define CHTHOLLY_CODEGEN_H

#include "AST.h"
#include <string>
#include <vector>
#include <memory>

class CodeGen : public ExprVisitor<std::string>, public StmtVisitor<std::string> {
public:
    std::string generate(const std::vector<std::shared_ptr<Stmt>>& statements);

    std::string visitBinaryExpr(const std::shared_ptr<Binary>& expr) override;
    std::string visitGroupingExpr(const std::shared_ptr<Grouping>& expr) override;
    std::string visitLiteralExpr(const std::shared_ptr<Literal>& expr) override;
    std::string visitUnaryExpr(const std::shared_ptr<Unary>& expr) override;
    std::string visitVariableExpr(const std::shared_ptr<Variable>& expr) override;
    std::string visitAssignExpr(const std::shared_ptr<Assign>& expr) override;
    std::string visitCallExpr(const std::shared_ptr<Call>& expr) override;
    std::string visitGetExpr(const std::shared_ptr<Get>& expr) override;
    std::string visitSetExpr(const std::shared_ptr<Set>& expr) override;
    std::string visitSelfExpr(const std::shared_ptr<Self>& expr) override;

    std::string visitExpressionStmt(const std::shared_ptr<Expression>& stmt) override;
    std::string visitVarStmt(const std::shared_ptr<Var>& stmt) override;

    std::string visitExpressionStmt(const std::shared_ptr<Expression>& stmt, bool semicolon);
    std::string visitVarStmt(const std::shared_ptr<Var>& stmt, bool semicolon);

    std::string visitBlockStmt(const std::shared_ptr<Block>& stmt) override;
    std::string visitIfStmt(const std::shared_ptr<If>& stmt) override;
    std::string visitWhileStmt(const std::shared_ptr<While>& stmt) override;
    std::string visitFuncStmt(const std::shared_ptr<Func>& stmt) override;
    std::string visitReturnStmt(const std::shared_ptr<Return>& stmt) override;
    std::string visitForStmt(const std::shared_ptr<For>& stmt) override;
    std::string visitSwitchStmt(const std::shared_ptr<Switch>& stmt) override;
    std::string visitBreakStmt(const std::shared_ptr<Break>& stmt) override;
    std::string visitFallthroughStmt(const std::shared_ptr<Fallthrough>& stmt) override;
    std::string visitStructStmt(const std::shared_ptr<Struct>& stmt) override;
};

#endif //CHTHOLLY_CODEGEN_H
