#ifndef CHTHOLLY_EXPR_H
#define CHTHOLLY_EXPR_H

#include "../Token.h"
#include <any>
#include <memory>
#include <vector>

class Expr;
class UnaryExpr;
class BinaryExpr;
class GroupingExpr;
class VariableExpr;

#include "../Type.h"

// Visitor interface for expressions
class ExprVisitor {
public:
    virtual std::shared_ptr<Type> visit(const class BooleanLiteral& expr) = 0;
    virtual std::shared_ptr<Type> visit(const class NumericLiteral& expr) = 0;
    virtual std::shared_ptr<Type> visit(const class StringLiteral& expr) = 0;
    virtual std::shared_ptr<Type> visit(const UnaryExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const BinaryExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const GroupingExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const VariableExpr& expr) = 0;
    virtual ~ExprVisitor() = default;
};


class Expr {
public:
    virtual std::shared_ptr<Type> accept(ExprVisitor& visitor) const = 0;
    virtual ~Expr() = default;
};

// Concrete expression classes
class NumericLiteral : public Expr {
public:
    NumericLiteral(Token value) : value(std::move(value)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token value;
};

class BooleanLiteral : public Expr {
public:
    BooleanLiteral(Token value) : value(std::move(value)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token value;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};

class VariableExpr : public Expr {
public:
    VariableExpr(Token name) : name(std::move(name)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token name;
};

class StringLiteral : public Expr {
public:
    StringLiteral(Token value) : value(std::move(value)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token value;
};


#endif // CHTHOLLY_EXPR_H
