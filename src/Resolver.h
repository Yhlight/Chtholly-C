#ifndef CHTHOLLY_RESOLVER_H
#define CHTHOLLY_RESOLVER_H

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <unordered_map>
#include <string>

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visit(const NumericLiteral& expr) override;
    std::any visit(const UnaryExpr& expr) override;
    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GroupingExpr& expr) override;
    std::any visit(const VariableExpr& expr) override;

    std::any visit(const LetStmt& stmt) override;
    std::any visit(const FuncStmt& stmt) override;
    std::any visit(const BlockStmt& stmt) override;
    std::any visit(const ReturnStmt& stmt) override;

private:
    void beginScope();
    void endScope();
    void declare(const Token& name);
    void define(const Token& name);
    void resolve(const Stmt& stmt);
    void resolve(const Expr& expr);

    std::vector<std::unordered_map<std::string, bool>> scopes_;
};

#endif // CHTHOLLY_RESOLVER_H
