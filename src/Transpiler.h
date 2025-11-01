#ifndef CHTHOLLY_TRANSPILER_H
#define CHTHOLLY_TRANSPILER_H

#include <string>
#include <vector>
#include <sstream>
#include "AST/Expr.h"
#include "AST/Stmt.h"

class Transpiler : public ExprVisitor, public StmtVisitor {
public:
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    void visitAssignExpr(const Assign& expr) override;
    void visitBinaryExpr(const Binary& expr) override;
    void visitCallExpr(const Call& expr) override;
    void visitGroupingExpr(const Grouping& expr) override;
    void visitLiteralExpr(const Literal& expr) override;
    void visitUnaryExpr(const Unary& expr) override;
    void visitVariableExpr(const Variable& expr) override;

    void visitBlockStmt(const Block& stmt) override;
    void visitExpressionStmt(const ExpressionStmt& stmt) override;
    void visitFuncStmt(const FuncStmt& stmt) override;
    void visitLetStmt(const LetStmt& stmt) override;
    void visitPrintStmt(const PrintStmt& stmt) override;
    void visitReturnStmt(const ReturnStmt& stmt) override;
    void visitIfStmt(const IfStmt& stmt) override;
    void visitWhileStmt(const WhileStmt& stmt) override;

    std::stringstream out_;
    int indent_level_ = 0;
    void indent() { indent_level_++; }
    void dedent() { indent_level_--; }
    void write_indent();
};

#endif // CHTHOLLY_TRANSPILER_H
