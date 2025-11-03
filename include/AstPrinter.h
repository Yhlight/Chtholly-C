#pragma once

#include "AST.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class AstPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::shared_ptr<Stmt>& stmt);
    void visitExpressionStmt(const std::shared_ptr<ExpressionStmt>& stmt) override;
    void visitPrintStmt(const std::shared_ptr<PrintStmt>& stmt) override;
    void visitVarStmt(const std::shared_ptr<VarStmt>& stmt) override;

    std::any visitBinaryExpr(const std::shared_ptr<Binary>& expr) override;
    std::any visitGroupingExpr(const std::shared_ptr<Grouping>& expr) override;
    std::any visitLiteralExpr(const std::shared_ptr<Literal>& expr) override;
    std::any visitUnaryExpr(const std::shared_ptr<Unary>& expr) override;

    std::any visitVariableExpr(const std::shared_ptr<Variable>& expr) override;
    std::any visitAssignExpr(const std::shared_ptr<Assign>& expr) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs);
    std::stringstream ss;
};
