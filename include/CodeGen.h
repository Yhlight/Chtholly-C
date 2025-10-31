#ifndef CHTHOLLY_CODEGEN_H
#define CHTHOLLY_CODEGEN_H

#include "AST.h"
#include <string>
#include <vector>
#include <memory>

class CodeGen : public ExprVisitor<std::string>, public StmtVisitor<std::string> {
public:
    std::string generate(const std::vector<std::shared_ptr<Stmt>>& statements);

    std::string visitBinaryExpr(const std::shared_ptr<Binary>& expr) override;
    std::string visitGroupingExpr(const std::shared_ptr<Grouping>& expr) override;
    std::string visitLiteralExpr(const std::shared_ptr<Literal>& expr) override;
    std::string visitUnaryExpr(const std::shared_ptr<Unary>& expr) override;
    std::string visitVariableExpr(const std::shared_ptr<Variable>& expr) override;
    std::string visitAssignExpr(const std::shared_ptr<Assign>& expr) override;

    std::string visitExpressionStmt(const std::shared_ptr<Expression>& stmt) override;
    std::string visitPrintStmt(const std::shared_ptr<Print>& stmt) override;
    std::string visitVarStmt(const std::shared_ptr<Var>& stmt) override;
    std::string visitBlockStmt(const std::shared_ptr<Block>& stmt) override;
};

#endif //CHTHOLLY_CODEGEN_H
