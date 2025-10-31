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

class BorrowExpr;
class CallExpr;
class AssignExpr;

// Visitor interface for expressions
class ExprVisitor {
public:
    virtual std::shared_ptr<Type> visit(const class NumericLiteral& expr) = 0;
    virtual std::shared_ptr<Type> visit(const class StringLiteral& expr) = 0;
    virtual std::shared_ptr<Type> visit(const UnaryExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const BinaryExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const GroupingExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const VariableExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const BorrowExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const CallExpr& expr) = 0;
    virtual std::shared_ptr<Type> visit(const AssignExpr& expr) = 0;
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

class BorrowExpr : public Expr {
public:
    BorrowExpr(std::unique_ptr<Expr> expr, bool isMutable)
        : expression(std::move(expr)), isMutable(isMutable) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
    const bool isMutable;
};

class CallExpr : public Expr {
public:
    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> callee;
    const Token paren; // For error reporting
    const std::vector<std::unique_ptr<Expr>> arguments;
};

class AssignExpr : public Expr {
public:
    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::shared_ptr<Type> accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> value;
};


#endif // CHTHOLLY_EXPR_H
