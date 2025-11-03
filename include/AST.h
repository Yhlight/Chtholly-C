#pragma once

#include <vector>
#include <string>
#include <any>
#include <memory>
#include "Token.h"

// Forward declare all AST node types and the visitor
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;

class ExprVisitor {
public:
    virtual std::any visitBinaryExpr(const std::shared_ptr<Binary>& expr) = 0;
    virtual std::any visitGroupingExpr(const std::shared_ptr<Grouping>& expr) = 0;
    virtual std::any visitLiteralExpr(const std::shared_ptr<Literal>& expr) = 0;
    virtual std::any visitUnaryExpr(const std::shared_ptr<Unary>& expr) = 0;
    virtual std::any visitVariableExpr(const std::shared_ptr<Variable>& expr) = 0;
    virtual std::any visitAssignExpr(const std::shared_ptr<Assign>& expr) = 0;
    virtual ~ExprVisitor() = default;
};

class Expr {
public:
    virtual std::any accept(ExprVisitor& visitor) = 0;
    virtual ~Expr() = default;
};

struct Binary : Expr, public std::enable_shared_from_this<Binary> {
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }

    const std::shared_ptr<Expr> left;
    const Token op;
    const std::shared_ptr<Expr> right;
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
    Grouping(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitGroupingExpr(shared_from_this());
    }

    const std::shared_ptr<Expr> expression;
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
    Literal(std::any value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLiteralExpr(shared_from_this());
    }

    const std::any value;
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
    Unary(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitUnaryExpr(shared_from_this());
    }

    const Token op;
    const std::shared_ptr<Expr> right;
};

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
    Variable(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitVariableExpr(shared_from_this());
    }

    const Token name;
};

struct Assign : Expr, public std::enable_shared_from_this<Assign> {
    Assign(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitAssignExpr(shared_from_this());
    }

    const Token name;
    const std::shared_ptr<Expr> value;
};


// Forward declare all statement types
struct ExpressionStmt;
struct PrintStmt;
struct VarStmt;

class StmtVisitor {
public:
    virtual void visitExpressionStmt(const std::shared_ptr<ExpressionStmt>& stmt) = 0;
    virtual void visitPrintStmt(const std::shared_ptr<PrintStmt>& stmt) = 0;
    virtual void visitVarStmt(const std::shared_ptr<VarStmt>& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

class Stmt {
public:
    virtual void accept(StmtVisitor& visitor) = 0;
    virtual ~Stmt() = default;
};

struct ExpressionStmt : Stmt, public std::enable_shared_from_this<ExpressionStmt> {
    ExpressionStmt(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitExpressionStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> expression;
};

struct PrintStmt : Stmt, public std::enable_shared_from_this<PrintStmt> {
    PrintStmt(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitPrintStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> expression;
};

struct VarStmt : Stmt, public std::enable_shared_from_this<VarStmt> {
    VarStmt(Token keyword, Token name, std::shared_ptr<Expr> initializer)
        : keyword(std::move(keyword)), name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitVarStmt(shared_from_this());
    }

    const Token keyword;
    const Token name;
    const std::shared_ptr<Expr> initializer;
};
