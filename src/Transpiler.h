#ifndef CHTHOLLY_TRANSPILER_H
#define CHTHOLLY_TRANSPILER_H

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <vector>

class Transpiler : public ExprVisitor, public StmtVisitor {
public:
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visit(const NumericLiteral& expr) override;
    std::any visit(const UnaryExpr& expr) override;
    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GroupingExpr& expr) override;
    std::any visit(const LetStmt& stmt) override;
};

#endif // CHTHOLLY_TRANSPILER_H
