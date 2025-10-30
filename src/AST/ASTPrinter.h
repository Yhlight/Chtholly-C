#ifndef CHTHOLLY_ASTPRINTER_H
#define CHTHOLLY_ASTPRINTER_H

#include "Expr.h"
#include "Stmt.h"
#include <string>
#include <vector>

class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visit(const NumericLiteral& expr) override;
    std::any visit(const LetStmt& stmt) override;

private:
    std::string parenthesize(const std::string& name, const Expr& expr);
    std::string parenthesize(const std::string& name, const Stmt& stmt);

    std::string result_;
};

#endif // CHTHOLLY_ASTPRINTER_H
