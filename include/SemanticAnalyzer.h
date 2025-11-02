#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "AST.h"
#include <vector>
#include <string>
#include <map>

class SemanticAnalyzer : public StmtVisitor, public ExprVisitor {
public:
    std::vector<std::string> analyze(const std::vector<std::shared_ptr<Stmt>>& statements);

private:
    // Scope management
    std::map<std::string, bool> moved_vars;
    std::vector<std::string> errors;

    // Visitor methods
    void visitExpressionStmt(std::shared_ptr<Expression> stmt) override;
    void visitVarStmt(std::shared_ptr<Var> stmt) override;
    void visitBlockStmt(std::shared_ptr<Block> stmt) override;
    void visitFuncStmt(std::shared_ptr<Func> stmt) override;
    void visitReturnStmt(std::shared_ptr<Return> stmt) override;
    void visitStructStmt(std::shared_ptr<Struct> stmt) override;

    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;
    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;
    std::any visitCallExpr(std::shared_ptr<Call> expr) override;
    std::any visitGetExpr(std::shared_ptr<Get> expr) override;
    std::any visitInstantiationExpr(_Pragma("any") std::shared_ptr<Instantiation> expr) override;
};

#endif //CHTHOLLY_SEMANTICANALYZER_H
