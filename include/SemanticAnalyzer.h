#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include <vector>
#include <string>
#include <memory>
#include "AST.h"
#include "Environment.h"

class SemanticAnalyzer : public ExprVisitor, public StmtVisitor {
public:
    SemanticAnalyzer();

    void analyze(const std::vector<std::shared_ptr<Stmt>>& statements);
    const std::vector<std::string>& getErrors() const;

    // Visitor methods for expressions
    std::any visitBinaryExpr(const std::shared_ptr<Binary>& expr) override;
    std::any visitGroupingExpr(const std::shared_ptr<Grouping>& expr) override;
    std::any visitLiteralExpr(const std::shared_ptr<Literal>& expr) override;
    std::any visitUnaryExpr(const std::shared_ptr<Unary>& expr) override;
    std::any visitVariableExpr(const std::shared_ptr<Variable>& expr) override;
    std::any visitAssignExpr(const std::shared_ptr<Assign>& expr) override;
    std::any visitLogicalExpr(const std::shared_ptr<Logical>& expr) override;
    std::any visitCallExpr(const std::shared_ptr<Call>& expr) override;
    std::any visitGetExpr(const std::shared_ptr<Get>& expr) override;
    std::any visitSetExpr(const std::shared_ptr<Set>& expr) override;

    // Visitor methods for statements
    void visitExpressionStmt(const std::shared_ptr<Expression>& stmt) override;
    void visitPrintStmt(const std::shared_ptr<Print>& stmt) override;
    void visitVarStmt(const std::shared_ptr<Var>& stmt) override;
    void visitBlockStmt(const std::shared_ptr<Block>& stmt) override;
    void visitIfStmt(const std::shared_ptr<If>& stmt) override;
    void visitWhileStmt(const std::shared_ptr<While>& stmt) override;
    void visitFunctionStmt(const std::shared_ptr<Function>& stmt) override;
    void visitReturnStmt(const std::shared_ptr<Return>& stmt) override;
    void visitStructStmt(const std::shared_ptr<Struct>& stmt) override;

private:
    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);
    void check(const std::shared_ptr<Stmt>& stmt);
    std::any evaluate(const std::shared_ptr<Expr>& expr);

    std::shared_ptr<Environment> environment;
    std::vector<std::string> errors;
};

#endif //CHTHOLLY_SEMANTICANALYZER_H
