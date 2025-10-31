#ifndef CHTHOLLY_AST_PRINTER_H
#define CHTHOLLY_AST_PRINTER_H

#include <string>
#include <vector>
#include "Expr.h"
#include "Stmt.h"

class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements);

    void visitBinaryExpr(const Binary& expr) override;
    void visitGroupingExpr(const Grouping& expr) override;
    void visitLiteralExpr(const Literal& expr) override;
    void visitUnaryExpr(const Unary& expr) override;
    void visitVariableExpr(const Variable& expr) override;

    void visitBlockStmt(const Block& stmt) override;
    void visitExpressionStmt(const ExpressionStmt& stmt) override;
    void visitLetStmt(const LetStmt& stmt) override;
    void visitPrintStmt(const PrintStmt& stmt) override;

private:
    void parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);

    std::string result_;
};

#endif // CHTHOLLY_AST_PRINTER_H
