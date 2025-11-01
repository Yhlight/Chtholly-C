#pragma once

#include "AST.h"
#include <string>
#include <any>

namespace chtholly {

class AstPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const Stmt& stmt);
    std::string print(const Expr& expr);

    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GroupingExpr& expr) override;
    std::any visit(const LiteralExpr& expr) override;
    std::any visit(const UnaryExpr& expr) override;
    std::any visit(const VariableExpr& expr) override;

    std::any visit(const ExpressionStmt& stmt) override;
    std::any visit(const VarDeclStmt& stmt) override;
    std::any visit(const BlockStmt& stmt) override;
};

} // namespace chtholly
