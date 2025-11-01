#ifndef CHTHOLLY_ASTPRINTER_H
#define CHTHOLLY_ASTPRINTER_H

#include "AST.h"
#include <string>
#include <any>

class AstPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::vector<std::shared_ptr<Stmt>>& statements);
    std::string print(const std::shared_ptr<Expr>& expr);
    std::string print(const std::shared_ptr<Stmt>& stmt);

    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;
    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;
    std::any visitCallExpr(std::shared_ptr<Call> expr) override;
    std::any visitGetExpr(std::shared_ptr<Get> expr) override;
    std::any visitInstantiationExpr(std::shared_ptr<Instantiation> expr) override;

    void visitExpressionStmt(std::shared_ptr<Expression> stmt) override;
    void visitVarStmt(std::shared_ptr<Var> stmt) override;
    void visitBlockStmt(std::shared_ptr<Block> stmt) override;
    void visitFuncStmt(std::shared_ptr<Func> stmt) override;
    void visitReturnStmt(std::shared_ptr<Return> stmt) override;
    void visitStructStmt(std::shared_ptr<Struct> stmt) override;

private:
    std::string result;
    std::string parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs);
    void parenthesize_stmt(const std::string& name, const std::vector<std::shared_ptr<Stmt>>& stmts);
};

#endif //CHTHOLLY_ASTPRINTER_H
