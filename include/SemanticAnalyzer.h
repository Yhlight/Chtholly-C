#pragma once

#include "AST.h"
#include "Environment.h"
#include <vector>
#include <string>

class SemanticAnalyzer : public ExprVisitor, public StmtVisitor {
public:
    SemanticAnalyzer();
    void analyze(const std::vector<std::shared_ptr<Stmt>>& statements);
    const std::vector<std::string>& getErrors() const;

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
    void analyze(const std::shared_ptr<Stmt>& stmt);
    void analyze(const std::shared_ptr<Expr>& expr);

    std::shared_ptr<Environment> environment;
    std::vector<std::string> errors;
};
