#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "Token.h"
#include <vector>
#include <memory>
#include <any>

// Forward declarations for all AST node types
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Call;

// Visitor interface for expressions
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr) = 0;
    virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;
    virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr) = 0;
    virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr) = 0;
    virtual std::any visitVariableExpr(std::shared_ptr<Variable> expr) = 0;
    virtual std::any visitAssignExpr(std::shared_ptr<Assign> expr) = 0;
    virtual std::any visitCallExpr(std::shared_ptr<Call> expr) = 0;
};

// Base class for all expressions
struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

// Forward declarations for statements
struct Expression;
struct Var;
struct Block;
struct Func;
struct Return;

// Visitor interface for statements
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitExpressionStmt(std::shared_ptr<Expression> stmt) = 0;
    virtual void visitVarStmt(std::shared_ptr<Var> stmt) = 0;
    virtual void visitBlockStmt(std::shared_ptr<Block> stmt) = 0;
    virtual void visitFuncStmt(std::shared_ptr<Func> stmt) = 0;
    virtual void visitReturnStmt(std::shared_ptr<Return> stmt) = 0;
};

// Base class for all statements
struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

// --- Expression Nodes ---
struct Binary : Expr, public std::enable_shared_from_this<Binary> {
    const std::shared_ptr<Expr> left;
    const Token op;
    const std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
    const std::shared_ptr<Expr> expression;

    Grouping(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitGroupingExpr(shared_from_this());
    }
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
    const std::any value;

    Literal(std::any value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLiteralExpr(shared_from_this());
    }
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
    const Token op;
    const std::shared_ptr<Expr> right;

    Unary(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitUnaryExpr(shared_from_this());
    }
};

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
    const Token name;

    Variable(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitVariableExpr(shared_from_this());
    }
};

struct Assign : Expr, public std::enable_shared_from_this<Assign> {
    const Token name;
    const std::shared_ptr<Expr> value;

    Assign(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitAssignExpr(shared_from_this());
    }
};

// --- Statement Nodes ---
struct Expression : Stmt, public std::enable_shared_from_this<Expression> {
    const std::shared_ptr<Expr> expression;

    Expression(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitExpressionStmt(shared_from_this());
    }
};

struct Var : Stmt, public std::enable_shared_from_this<Var> {
    const Token name;
    const std::shared_ptr<Expr> initializer;
    const bool isMutable;

    Var(Token name, std::shared_ptr<Expr> initializer, bool isMutable)
        : name(std::move(name)), initializer(std::move(initializer)), isMutable(isMutable) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitVarStmt(shared_from_this());
    }
};

struct Block : Stmt, public std::enable_shared_from_this<Block> {
    const std::vector<std::shared_ptr<Stmt>> statements;

    Block(std::vector<std::shared_ptr<Stmt>> statements) : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitBlockStmt(shared_from_this());
    }
};

struct Func : Stmt, public std::enable_shared_from_this<Func> {
    const Token name;
    const std::vector<Token> params;
    const std::vector<std::shared_ptr<Stmt>> body;

    Func(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitFuncStmt(shared_from_this());
    }
};

struct Return : Stmt, public std::enable_shared_from_this<Return> {
    const Token keyword;
    const std::shared_ptr<Expr> value;

    Return(Token keyword, std::shared_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitReturnStmt(shared_from_this());
    }
};

struct Call : Expr, public std::enable_shared_from_this<Call> {
    const std::shared_ptr<Expr> callee;
    const Token paren; // For error reporting.
    const std::vector<std::shared_ptr<Expr>> arguments;

    Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitCallExpr(shared_from_this());
    }
};

#endif //CHTHOLLY_AST_H
