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

    std::any visit(const NumericLiteral& expr) override;
    std::any visit(const UnaryExpr& expr) override;
    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GroupingExpr& expr) override;
    std::any visit(const LetStmt& stmt) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);

    std::string result_;
};

#endif // CHTHOLLY_ASTPRINTER_H
