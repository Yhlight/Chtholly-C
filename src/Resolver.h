#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <map>
#include <string>

struct VariableState {
    bool defined = false;
    bool is_mutable = false;
    int immutable_borrows = 0;
    bool moved = false;
};

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

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

    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitPrintStmt(const PrintStmt& stmt) override;
    std::any visitLetStmt(const LetStmt& stmt) override;
    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt) override;
    std::any visitReturnStmt(const ReturnStmt& stmt) override;
    std::any visitStructStmt(const StructStmt& stmt) override;

private:
    enum class FunctionType {
        NONE,
        FUNCTION
    };

    enum class ClassType {
        NONE,
        CLASS
    };

    void resolve(const Stmt& stmt);
    void resolve(const Expr& expr);
    void resolveFunction(const FunctionStmt& function, FunctionType type);

    void beginScope();
    void endScope();

    void declare(const Token& name);
    void define(const Token& name);
    void resolveLocal(const Expr& expr, const Token& name);

    std::vector<std::map<std::string, VariableState>> scopes;
    FunctionType currentFunction = FunctionType::NONE;
    ClassType currentClass = ClassType::NONE;
};
