#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <map>
#include <set>
#include "Resolver.h"

class Transpiler : public ExprVisitor, public StmtVisitor {
public:
    Transpiler(const Resolver& resolver) : resolver(resolver) {}
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements, bool is_module = false);

private:
    const Resolver& resolver;
    std::string evaluate(const Expr& expr);
    void execute(const Stmt& stmt);

    std::stringstream out;
    std::map<std::string, std::vector<const ImplStmt*>> impls;
    std::set<std::string> transpiled_files;
    std::map<TokenType, std::string> op_to_method = {
        {TokenType::PLUS, "add"},
        {TokenType::MINUS, "sub"},
        {TokenType::STAR, "mul"},
        {TokenType::SLASH, "div"}
    };

    std::map<TokenType, std::string> op_to_trait = {
        {TokenType::PLUS, "Add"},
        {TokenType::MINUS, "Sub"},
        {TokenType::STAR, "Mul"},
        {TokenType::SLASH, "Div"}
    };

    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitCallExpr(const CallExpr& expr) override;
    std::any visitGetExpr(const GetExpr& expr) override;
    std::any visitSetExpr(const SetExpr& expr) override;
    std::any visitBorrowExpr(const BorrowExpr& expr) override;
    std::any visitLambdaExpr(const LambdaExpr& expr) override;
    std::any visitStructInitializerExpr(const StructInitializerExpr& expr) override;

    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitLetStmt(const LetStmt& stmt) override;
    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName = std::nullopt);
    std::any visitReturnStmt(const ReturnStmt& stmt) override;
    std::any visitStructStmt(const StructStmt& stmt) override;
    std::any visitTraitStmt(const TraitStmt& stmt) override;
    std::any visitImplStmt(const ImplStmt& stmt) override;
    std::any visitImportStmt(const ImportStmt& stmt) override;
};
