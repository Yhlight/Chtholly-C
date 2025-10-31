#ifndef CHTHOLLY_TRANSPILER_H
#define CHTHOLLY_TRANSPILER_H

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <vector>

class Transpiler : public ExprVisitor, public StmtVisitor {
public:
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::shared_ptr<Type> visit(const NumericLiteral& expr) override;
    std::shared_ptr<Type> visit(const StringLiteral& expr) override;
    std::shared_ptr<Type> visit(const UnaryExpr& expr) override;
    std::shared_ptr<Type> visit(const BinaryExpr& expr) override;
    std::shared_ptr<Type> visit(const GroupingExpr& expr) override;
    std::shared_ptr<Type> visit(const VariableExpr& expr) override;

    void visit(const LetStmt& stmt) override;
    void visit(const FuncStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;

private:
    std::string result_;
};

#endif // CHTHOLLY_TRANSPILER_H
