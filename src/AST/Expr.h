#pragma once

#include "../Token.h"
#include <any>
#include <memory>
#include <vector>

struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct VariableExpr;
struct AssignExpr;

class ExprVisitor {
public:
    virtual std::any visitBinaryExpr(const BinaryExpr& expr) = 0;
    virtual std::any visitGroupingExpr(const GroupingExpr& expr) = 0;
    virtual std::any visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual std::any visitUnaryExpr(const UnaryExpr& expr) = 0;
    virtual std::any visitVariableExpr(const VariableExpr& expr) = 0;
    virtual std::any visitAssignExpr(const AssignExpr& expr) = 0;
    virtual ~ExprVisitor() = default;
};

class Expr {
public:
    virtual std::any accept(ExprVisitor& visitor) const = 0;
    virtual ~Expr() = default;
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

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
    std::variant<std::monostate, std::string, double, bool> value;

    explicit LiteralExpr(std::variant<std::monostate, std::string, double, bool> value)
        : value(value) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }
};

struct UnaryExpr : Expr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }
};

struct VariableExpr : Expr {
    Token name;

    explicit VariableExpr(Token name) : name(name) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitVariableExpr(*this);
    }
};

struct AssignExpr : Expr {
    Token name;
    std::unique_ptr<Expr> value;

    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitAssignExpr(*this);
    }
};
