#ifndef CHTHOLLY_EXPR_H
#define CHTHOLLY_EXPR_H

#include "../Token.h"
#include <any>
#include <memory>
#include <vector>

class Expr;

// Visitor interface for expressions
class ExprVisitor {
public:
    virtual std::any visit(const class NumericLiteral& expr) = 0;
    virtual ~ExprVisitor() = default;
};


class Expr {
public:
    virtual std::any accept(ExprVisitor& visitor) const = 0;
    virtual ~Expr() = default;
};

// Concrete expression classes
class NumericLiteral : public Expr {
public:
    NumericLiteral(Token value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token value;
};


#endif // CHTHOLLY_EXPR_H
