#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <map>
#include <string>
#include "TypeInfo.h"

struct VariableState {
    bool defined = false;
    bool is_mutable = false;
    int immutable_borrows = 0;
    bool moved = false;
    std::optional<TypeInfo> type;
};

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    Resolver();
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);
    const std::map<std::string, VariableState>& get_resolved_symbols() const;

    std::string visitBinaryExpr(const BinaryExpr& expr) override;
    std::string visitGroupingExpr(const GroupingExpr& expr) override;
    std::string visitLiteralExpr(const LiteralExpr& expr) override;
    std::string visitUnaryExpr(const UnaryExpr& expr) override;
    std::string visitVariableExpr(const VariableExpr& expr) override;
    std::string visitAssignExpr(const AssignExpr& expr) override;
    std::string visitCallExpr(const CallExpr& expr) override;
    std::string visitGetExpr(const GetExpr& expr) override;
    std::string visitSetExpr(const SetExpr& expr) override;
    std::string visitBorrowExpr(const BorrowExpr& expr) override;
    std::string visitLambdaExpr(const LambdaExpr& expr) override;
    std::string visitStructInitializerExpr(const StructInitializerExpr& expr) override;

    std::string visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::string visitLetStmt(const LetStmt& stmt) override;
    std::string visitBlockStmt(const BlockStmt& stmt) override;
    std::string visitIfStmt(const IfStmt& stmt) override;
    std::string visitWhileStmt(const WhileStmt& stmt) override;
    std::string visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName = std::nullopt) override;
    std::string visitReturnStmt(const ReturnStmt& stmt) override;
    std::string visitStructStmt(const StructStmt& stmt) override;
    std::string visitSwitchStmt(const SwitchStmt& stmt) override;
    std::string visitBreakStmt(const BreakStmt& stmt) override;
    std::string visitFallthroughStmt(const FallthroughStmt& stmt) override;
    std::string visitTraitStmt(const TraitStmt& stmt) override;
    std::string visitImplStmt(const ImplStmt& stmt) override;
    std::string visitImportStmt(const ImportStmt& stmt) override;
    std::string visitEnumStmt(const EnumStmt& stmt) override;
    const std::vector<std::unique_ptr<Stmt>>& get_statements() const;

private:
    std::map<std::string, const TraitStmt*> traits;
    std::map<std::string, const StructStmt*> structs;
    std::map<std::string, const EnumStmt*> enums;
    std::map<std::string, std::map<std::string, VariableState>> std_modules;

    enum class FunctionType {
        NONE,
        FUNCTION
    };

    enum class ClassType {
        NONE,
        CLASS
    };

    enum class LoopType {
        NONE,
        LOOP,
        SWITCH
    };

    void resolve(const Stmt& stmt);
    void resolve(const Expr& expr);
    void resolveFunction(const FunctionStmt& function, FunctionType type);
    void resolveLambda(const LambdaExpr& lambda, FunctionType type);

    void beginScope();
    void endScope();

    void declare(const Token& name);
    void define(const Token& name);
    void resolveLocal(const Expr& expr, const Token& name);

    std::vector<std::map<std::string, VariableState>> scopes;
    FunctionType currentFunction = FunctionType::NONE;
    ClassType currentClass = ClassType::NONE;
    LoopType currentLoop = LoopType::NONE;
    std::vector<std::unique_ptr<Stmt>> statements;
};
