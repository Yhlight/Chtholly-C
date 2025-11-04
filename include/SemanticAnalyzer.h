#ifndef CHTHOLLY_SEMANTIC_ANALYZER_H
#define CHTHOLLY_SEMANTIC_ANALYZER_H

#include "AST.h"
#include "Environment.h"
#include <vector>
#include <memory>

class SemanticAnalyzer : public ExprVisitor, public StmtVisitor {
public:
    SemanticAnalyzer();
    void analyze(const std::vector<std::unique_ptr<Stmt>>& statements);

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
    std::shared_ptr<Environment> environment;
    void error(const Token& token, const std::string& message);
};

#endif // CHTHOLLY_SEMANTIC_ANALYZER_H
