#pragma once

#include "../Token.h"
#include <memory>
#include <vector>
#include <any>

// Forward declarations
struct AssignExpr;
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct VariableExpr;
struct CallExpr;

// Visitor interface
struct ExprVisitor {
    virtual std::any visitAssignExpr(const AssignExpr& expr) = 0;
    virtual std::any visitBinaryExpr(const BinaryExpr& expr) = 0;
    virtual std::any visitGroupingExpr(const GroupingExpr& expr) = 0;
    virtual std::any visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual std::any visitUnaryExpr(const UnaryExpr& expr) = 0;
    virtual std::any visitVariableExpr(const VariableExpr& expr) = 0;
    virtual std::any visitCallExpr(const CallExpr& expr) = 0;
    virtual ~ExprVisitor() = default;
};

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

// Concrete expression classes
struct AssignExpr : Expr {
    Token name;
    std::unique_ptr<Expr> value;

    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitAssignExpr(*this);
    }
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }
};

struct GroupingExpr : Expr {
    std::unique_ptr<Expr> expression;

    explicit GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }
};

struct LiteralExpr : Expr {
    LiteralValue value;

    explicit LiteralExpr(LiteralValue value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }
};

struct UnaryExpr : Expr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }
};

struct VariableExpr : Expr {
    Token name;

    explicit VariableExpr(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitVariableExpr(*this);
    }
};

struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    Token paren; // For error reporting.
    std::vector<std::unique_ptr<Expr>> arguments;

    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitCallExpr(*this);
    }
};
