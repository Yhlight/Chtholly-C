#ifndef CHTHOLLY_RESOLVER_H
#define CHTHOLLY_RESOLVER_H

#include "AST.h"
#include "TypeSystem.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace chtholly {

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    Resolver();
    ~Resolver();
    bool hadError = false;

    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    void error(const Token& token, const std::string& message);

    void resolveStmt(const Stmt& stmt);
    std::any resolveExpr(const Expr& expr);

    void beginScope();
    void endScope();
    void declare(const Token& name);
    void define(const Token& name, std::shared_ptr<types::Type> type);
    std::shared_ptr<types::Type> lookup(const Token& name);

    std::vector<std::map<std::string, std::shared_ptr<types::Type>>> scopes;
    std::vector<std::map<std::string, bool>> defined_scopes;
    class TypeResolver;
    std::unique_ptr<TypeResolver> typeResolver;

    enum class FunctionType {
        NONE,
        FUNCTION
    };
    enum class ClassType {
        NONE,
        CLASS
    };
    FunctionType currentFunction = FunctionType::NONE;
    ClassType currentClass = ClassType::NONE;

    void resolveFunction(const FunctionStmt& function, FunctionType type);

    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitVarStmt(const VarStmt& stmt) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt) override;
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitReturnStmt(const ReturnStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitForStmt(const ForStmt& stmt) override;
    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitCallExpr(const CallExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitGetExpr(const GetExpr& expr) override;
    std::any visitSetExpr(const SetExpr& expr) override;
    std::any visitSelfExpr(const SelfExpr& expr) override;
    std::any visitStructStmt(const StructStmt& stmt) override;

    std::any visitImportStmt(const ImportStmt& stmt) override;
    std::any visitSwitchStmt(const SwitchStmt& stmt) override;
    std::any visitCaseStmt(const CaseStmt& stmt) override;
    std::any visitBreakStmt(const BreakStmt& stmt) override;
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override;
    std::any visitEnumStmt(const EnumStmt& stmt) override;
    std::any visitTraitStmt(const TraitStmt& stmt) override;
    std::any visitLambdaExpr(const LambdaExpr& expr) override;
    std::any visitBorrowExpr(const BorrowExpr& expr) override;
    std::any visitDerefExpr(const DerefExpr& expr) override;
    std::any visitStructLiteralExpr(const StructLiteralExpr& expr) override;
    std::any visitArrayLiteralExpr(const ArrayLiteralExpr& expr) override;
    std::any visitTypeCastExpr(const TypeCastExpr& expr) override;
};

} // namespace chtholly

#endif // CHTHOLLY_RESOLVER_H
