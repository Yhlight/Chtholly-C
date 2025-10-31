#pragma once

#include "Token.h"
#include <memory>
#include <any>

namespace chtholly {

// Forward declarations
struct LiteralExpr;

// Visitor interface
template <typename R>
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual R visit(const LiteralExpr& expr) = 0;
};

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any>& visitor) const = 0;
};

// Literal expression node
struct LiteralExpr : Expr {
    LiteralExpr(Token token) : token(std::move(token)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visit(*this);
    }

    const Token token;
};

} // namespace chtholly
