#ifndef CHTHOLLY_CODEGEN_H
#define CHTHOLLY_CODEGEN_H

#include "AST.h"
#include <string>
#include <vector>

class CodeGen : public ExprVisitor, public StmtVisitor {
public:
    std::string generate(const std::vector<std::unique_ptr<Stmt>>& statements);

    void visit(const BinaryExpr& expr) override;
    void visit(const GroupingExpr& expr) override;
    void visit(const LiteralExpr& expr) override;
    void visit(const UnaryExpr& expr) override;
    void visit(const VariableExpr& expr) override;
    void visit(const AssignmentExpr& expr) override;
    void visit(const CallExpr& expr) override;

    void visit(const BlockStmt& stmt) override;
    void visit(const ExpressionStmt& stmt) override;
    void visit(const FunctionStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;
    void visit(const VarDeclStmt& stmt) override;
    void visit(const WhileStmt& stmt) override;

private:
    std::string result;
};

#endif // CHTHOLLY_CODEGEN_H
