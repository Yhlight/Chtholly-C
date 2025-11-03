#pragma once

#include "AST.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <set>

class CodeGen : public ExprVisitor, public StmtVisitor {
public:
    std::string generate(const std::vector<std::shared_ptr<Stmt>>& statements);

    // Statement visitors
    void visitExpressionStmt(const std::shared_ptr<ExpressionStmt>& stmt) override;
    void visitPrintStmt(const std::shared_ptr<PrintStmt>& stmt) override;
    void visitVarStmt(const std::shared_ptr<VarStmt>& stmt) override;

    // Expression visitors
    std::any visitAssignExpr(const std::shared_ptr<Assign>& expr) override;
    std::any visitBinaryExpr(const std::shared_ptr<Binary>& expr) override;
    std::any visitGroupingExpr(const std::shared_ptr<Grouping>& expr) override;
    std::any visitLiteralExpr(const std::shared_ptr<Literal>& expr) override;
    std::any visitUnaryExpr(const std::shared_ptr<Unary>& expr) override;
    std::any visitVariableExpr(const std::shared_ptr<Variable>& expr) override;

private:
    std::stringstream declarations;
    std::stringstream main_body;
    std::set<std::string> headers;

    void generate(const std::shared_ptr<Stmt>& stmt);
    std::string generate(const std::shared_ptr<Expr>& expr);
};
