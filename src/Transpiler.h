#pragma once

#include "AST.h"
#include <string>
#include <sstream>

namespace chtholly {

class Transpiler : public ExprVisitor, public StmtVisitor, public TypeVisitor {
public:
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visitBaseTypeExpr(BaseTypeExpr& expr) override;
    std::any visitBinaryExpr(BinaryExpr& expr) override;
    std::any visitUnaryExpr(UnaryExpr& expr) override;
    std::any visitLiteralExpr(LiteralExpr& expr) override;
    std::any visitVariableExpr(VariableExpr& expr) override;

    std::any visitExpressionStmt(ExpressionStmt& stmt) override;
    std::any visitBlockStmt(BlockStmt& stmt) override;
    std::any visitVarStmt(VarStmt& stmt) override;

private:
    std::string transpile(Expr& expr);
    std::string transpile(TypeExpr& type);
    void transpile(Stmt& stmt);

    std::stringstream out;
};

} // namespace chtholly
