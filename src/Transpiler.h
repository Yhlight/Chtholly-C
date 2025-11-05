#ifndef CHTHOLLY_TRANSPILER_H
#define CHTHOLLY_TRANSPILER_H

#include "AST.h"
#include <string>
#include <vector>
#include <sstream>

namespace chtholly {

class Transpiler : public ExprVisitor, public StmtVisitor {
public:
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitCallExpr(const CallExpr& expr) override;
    std::any visitLambdaExpr(const LambdaExpr& expr) override;
    std::any visitGetExpr(const GetExpr& expr) override;
    std::any visitSetExpr(const SetExpr& expr) override;
    std::any visitSelfExpr(const SelfExpr& expr) override;
    std::any visitBorrowExpr(const BorrowExpr& expr) override;
    std::any visitDerefExpr(const DerefExpr& expr) override;

    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitVarStmt(const VarStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitReturnStmt(const ReturnStmt& stmt) override;
    std::any visitStructStmt(const StructStmt& stmt) override;
    std::any visitImportStmt(const ImportStmt& stmt) override;
    std::any visitSwitchStmt(const SwitchStmt& stmt) override;
    std::any visitCaseStmt(const CaseStmt& stmt) override;
    std::any visitBreakStmt(const BreakStmt& stmt) override;
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override;

private:
    std::string transpileType(const TypeExpr& type);
    std::stringstream out;
};

} // namespace chtholly

#endif // CHTHOLLY_TRANSPILER_H
