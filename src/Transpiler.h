#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <vector>
#include <any>
#include <sstream>

class Transpiler : public ExprVisitor, public StmtVisitor {
public:
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements);

    // Expression visitor methods
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;

    // Statement visitor methods
    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitPrintStmt(const PrintStmt& stmt) override;
    std::any visitLetStmt(const LetStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;

private:
    std::string transpile(const Expr& expr);
    std::string transpile(const Stmt& stmt);

    void indent();
    void dedent();
    void write_indent();

    std::stringstream out;
    int indent_level_ = 0;
};
