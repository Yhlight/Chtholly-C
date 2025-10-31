#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "Token.h"
#include <vector>
#include <memory>

// Forward declarations for visitor pattern
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;

// Visitor for expressions
template<typename R>
struct ExprVisitor {
    virtual R visitBinaryExpr(const std::shared_ptr<Binary>& expr) = 0;
    virtual R visitGroupingExpr(const std::shared_ptr<Grouping>& expr) = 0;
    virtual R visitLiteralExpr(const std::shared_ptr<Literal>& expr) = 0;
    virtual R visitUnaryExpr(const std::shared_ptr<Unary>& expr) = 0;
    virtual R visitVariableExpr(const std::shared_ptr<Variable>& expr) = 0;
    virtual R visitAssignExpr(const std::shared_ptr<Assign>& expr) = 0;
};

// Base class for all expressions
struct Expr {
    virtual ~Expr() = default;
    virtual std::string accept(ExprVisitor<std::string>& visitor) = 0;
};

// Concrete expression nodes
struct Binary : Expr, public std::enable_shared_from_this<Binary> {
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(left), op(op), right(right) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }

    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
    Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitGroupingExpr(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
    Literal(std::string value, TokenType type) : value(value), type(type) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitLiteralExpr(shared_from_this());
    }

    std::string value;
    TokenType type;
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
    Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitUnaryExpr(shared_from_this());
    }

    Token op;
    std::shared_ptr<Expr> right;
};

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
    Variable(Token name) : name(name) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitVariableExpr(shared_from_this());
    }

    Token name;
};

struct Assign : Expr, public std::enable_shared_from_this<Assign> {
    Assign(Token name, std::shared_ptr<Expr> value) : name(name), value(value) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitAssignExpr(shared_from_this());
    }

    Token name;
    std::shared_ptr<Expr> value;
};

// Forward declarations for statement visitor
struct Expression;
struct Print;
struct Var;
struct Block;
struct If;

// Visitor for statements
template<typename R>
struct StmtVisitor {
    virtual R visitExpressionStmt(const std::shared_ptr<Expression>& stmt) = 0;
    virtual R visitPrintStmt(const std::shared_ptr<Print>& stmt) = 0;
    virtual R visitVarStmt(const std::shared_ptr<Var>& stmt) = 0;
    virtual R visitBlockStmt(const std::shared_ptr<Block>& stmt) = 0;
    virtual R visitIfStmt(const std::shared_ptr<If>& stmt) = 0;
};

// Base class for all statements
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::string accept(StmtVisitor<std::string>& visitor) = 0;
};

// Concrete statement nodes
struct Expression : Stmt, public std::enable_shared_from_this<Expression> {
    Expression(std::shared_ptr<Expr> expression) : expression(expression) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitExpressionStmt(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct Print : Stmt, public std::enable_shared_from_this<Print> {
    Print(std::shared_ptr<Expr> expression) : expression(expression) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitPrintStmt(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct Var : Stmt, public std::enable_shared_from_this<Var> {
    Var(Token name, std::shared_ptr<Expr> initializer, bool is_mutable)
        : name(name), initializer(initializer), is_mutable(is_mutable) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitVarStmt(shared_from_this());
    }

    Token name;
    std::shared_ptr<Expr> initializer;
    bool is_mutable;
};

struct Block : Stmt, public std::enable_shared_from_this<Block> {
    Block(std::vector<std::shared_ptr<Stmt>> statements) : statements(statements) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitBlockStmt(shared_from_this());
    }

    std::vector<std::shared_ptr<Stmt>> statements;
};

struct If : Stmt, public std::enable_shared_from_this<If> {
    If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitIfStmt(shared_from_this());
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
};

#endif //CHTHOLLY_AST_H
