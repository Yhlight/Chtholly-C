#ifndef CHTHOLLY_CODEGEN_H
#define CHTHOLLY_CODEGEN_H

#include "AST.h"
#include <string>
#include <any>

#include <functional>
#include <map>

class CodeGen : public ExprVisitor, public StmtVisitor {
public:
    CodeGen();
    std::string generate(const std::vector<std::shared_ptr<Stmt>>& statements);

    // Expression visitors
    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;
    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;
    std::any visitCallExpr(std::shared_ptr<Call> expr) override;
    std::any visitGetExpr(std::shared_ptr<Get> expr) override;
    std::any visitInstantiationExpr(std::shared_ptr<Instantiation> expr) override;

    // Statement visitors
    void visitExpressionStmt(std::shared_ptr<Expression> stmt) override;
    void visitVarStmt(std::shared_ptr<Var> stmt) override;
    void visitBlockStmt(std::shared_ptr<Block> stmt) override;
    void visitFuncStmt(std::shared_ptr<Func> stmt) override;
    void visitReturnStmt(std::shared_ptr<Return> stmt) override;
    void visitStructStmt(std::shared_ptr<Struct> stmt) override;

private:
    std::string source;
    bool in_struct = false;
    std::map<std::string, std::function<std::string(const std::vector<std::shared_ptr<Expr>>&)>> builtins;

    std::string evaluate(const std::shared_ptr<Expr>& expr);
    void execute(const std::shared_ptr<Stmt>& stmt);
};

#endif //CHTHOLLY_CODEGEN_H
