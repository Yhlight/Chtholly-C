#pragma once

#include "AST.h"
#include <sstream>
#include <string>
#include <vector>

namespace chtholly {

class CodeGen : public StmtVisitor<void>, public ExprVisitor<std::string> {
public:
    std::string generate(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    void visit(const VarDeclStmt& stmt) override;
    void visit(const ExprStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
    void visit(const FuncDeclStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;
    void visit(const SwitchStmt& stmt) override;
    void visit(const CaseStmt& stmt) override;
    void visit(const BreakStmt& stmt) override;
    void visit(const FallthroughStmt& stmt) override;
    void visit(const StructDeclStmt& stmt) override;

    std::string visit(const LiteralExpr& expr) override;
    std::string visit(const UnaryExpr& expr) override;
    std::string visit(const BinaryExpr& expr) override;
    std::string visit(const VariableExpr& expr) override;
    std::string visit(const GroupingExpr& expr) override;
    std::string visit(const CallExpr& expr) override;
    std::string visit(const LambdaExpr& expr) override;
    std::string visit(const StructInitExpr& expr) override;
    std::string visit(const MemberAccessExpr& expr) override;

    std::string generateVarDecl(const VarDeclStmt& stmt);

    std::stringstream out;
};

} // namespace chtholly
