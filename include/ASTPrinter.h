#ifndef CHTHOLLY_ASTPRINTER_H
#define CHTHOLLY_ASTPRINTER_H

#include <string>
#include <memory>
#include "AST.h"

class ASTPrinter : public ExprVisitor {
public:
    std::string print(const std::shared_ptr<Expr>& expr);
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

private:
    std::string parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs);
};

#endif //CHTHOLLY_ASTPRINTER_H
