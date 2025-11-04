#ifndef CHTHOLLY_AST_PRINTER_H
#define CHTHOLLY_AST_PRINTER_H

#include "AST.h"
#include <string>
#include <vector>

class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements);

    void visit(const BinaryExpr& expr) override;
    void visit(const GroupingExpr& expr) override;
    void visit(const LiteralExpr& expr) override;
    void visit(const UnaryExpr& expr) override;
    void visit(const VariableExpr& expr) override;
    void visit(const AssignmentExpr& expr) override;
    void visit(const CallExpr& expr) override;
    void visit(const GetExpr& expr) override;
    void visit(const SetExpr& expr) override;
    void visit(const SelfExpr& expr) override;

    void visit(const BlockStmt& stmt) override;
    void visit(const ExpressionStmt& stmt) override;
    void visit(const FunctionStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;
    void visit(const VarDeclStmt& stmt) override;
    void visit(const WhileStmt& stmt) override;
    void visit(const StructStmt& stmt) override;

private:
    std::string result;
    void parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);
};

#endif // CHTHOLLY_AST_PRINTER_H
