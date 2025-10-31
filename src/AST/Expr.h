#ifndef CHTHOLLY_EXPR_H
#define CHTHOLLY_EXPR_H

#include <memory>
#include <vector>
#include "../Token.h"

// Forward declarations
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visitBinaryExpr(const Binary& expr) = 0;
    virtual void visitGroupingExpr(const Grouping& expr) = 0;
    virtual void visitLiteralExpr(const Literal& expr) = 0;
    virtual void visitUnaryExpr(const Unary& expr) = 0;
    virtual void visitVariableExpr(const Variable& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

class Binary : public Expr {
public:
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visitBinaryExpr(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

class Grouping : public Expr {
public:
    Grouping(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visitGroupingExpr(*this);
    }

    const std::unique_ptr<Expr> expression;
};

class Literal : public Expr {
public:
    Literal(Token value) : value(value) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visitLiteralExpr(*this);
    }

    const Token value;
};

class Unary : public Expr {
public:
    Unary(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visitUnaryExpr(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};

class Variable : public Expr {
public:
    Variable(Token name) : name(name) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visitVariableExpr(*this);
    }

    const Token name;
};

#endif // CHTHOLLY_EXPR_H
