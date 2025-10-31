#pragma once

#include "Token.h"
#include <memory>
#include <any>
#include <vector>
#include <string>
#include <stdexcept>

namespace chtholly {

// Forward declarations
struct LiteralExpr;
struct UnaryExpr;
struct BinaryExpr;

// Visitor interface
template <typename R>
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual R visit(const LiteralExpr& expr) = 0;
    virtual R visit(const UnaryExpr& expr) = 0;
    virtual R visit(const BinaryExpr& expr) = 0;
};

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
    template <typename R>
    R accept(ExprVisitor<R>& visitor) const {
        // This is a bit of a hack to avoid having to implement RTTI
        // or a base accept method that returns a generic type.
        // It works by downcasting to the concrete type and then calling
        // the appropriate visit method.
        if (auto p = dynamic_cast<const LiteralExpr*>(this)) {
            return visitor.visit(*p);
        }
        if (auto p = dynamic_cast<const UnaryExpr*>(this)) {
            return visitor.visit(*p);
        }
        if (auto p = dynamic_cast<const BinaryExpr*>(this)) {
            return visitor.visit(*p);
        }
        // Should be unreachable
        throw std::runtime_error("Unknown expression type in accept.");
    }
};

// Literal expression node
struct LiteralExpr : Expr {
    LiteralExpr(Token token) : token(std::move(token)) {}
    const Token token;
};

// Unary expression node
struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}
    const Token op;
    const std::unique_ptr<Expr> right;
};

// Binary expression node
struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};


// A simple AST printer for testing
class AstPrinter : public ExprVisitor<std::string> {
public:
    std::string print(const Expr& expr) {
        return expr.accept(*this);
    }

    std::string visit(const LiteralExpr& expr) override {
        return expr.token.lexeme;
    }

    std::string visit(const UnaryExpr& expr) override {
        return parenthesize(expr.op.lexeme, {expr.right.get()});
    }

    std::string visit(const BinaryExpr& expr) override {
        return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
    }

private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
        std::string result = "(" + name;
        for (const auto& expr : exprs) {
            result += " ";
            result += expr->accept(*this);
        }
        result += ")";
        return result;
    }
};


} // namespace chtholly
