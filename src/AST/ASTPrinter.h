#ifndef CHTHOLLY_ASTPRINTER_H
#define CHTHOLLY_ASTPRINTER_H

#include "Expr.h"
#include "Stmt.h"
#include <string>
#include <vector>

class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements);
    std::string print(const Expr& expr);

    std::shared_ptr<Type> visit(const NumericLiteral& expr) override;
    std::shared_ptr<Type> visit(const StringLiteral& expr) override;
    std::shared_ptr<Type> visit(const BooleanLiteral& expr) override;
    std::shared_ptr<Type> visit(const UnaryExpr& expr) override;
    std::shared_ptr<Type> visit(const BinaryExpr& expr) override;
    std::shared_ptr<Type> visit(const GroupingExpr& expr) override;
    std::shared_ptr<Type> visit(const VariableExpr& expr) override;
    std::shared_ptr<Type> visit(const BorrowExpr& expr) override;

    void visit(const LetStmt& stmt) override;
    void visit(const FuncStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;

private:
    std::string result_;
};

#endif // CHTHOLLY_ASTPRINTER_H
