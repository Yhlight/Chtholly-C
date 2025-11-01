#pragma once

#include "Expr.h"
#include "Stmt.h"
#include <string>
#include <vector>
#include <any>

class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const Expr& expr);
    std::string print(const Stmt& stmt);

    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;

    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitPrintStmt(const PrintStmt& stmt) override;
    std::any visitLetStmt(const LetStmt& stmt) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);
};
