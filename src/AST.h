#pragma once

#include "Token.h"
#include <memory>
#include <vector>
#include <any>

namespace chtholly {

// Forward declarations for all AST nodes
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct VariableExpr;

// Visitor for Expressions
class ExprVisitor {
public:
    virtual std::any visit(const BinaryExpr& expr) = 0;
    virtual std::any visit(const GroupingExpr& expr) = 0;
    virtual std::any visit(const LiteralExpr& expr) = 0;
    virtual std::any visit(const UnaryExpr& expr) = 0;
    virtual std::any visit(const VariableExpr& expr) = 0;
};

// Base class for all expression nodes
struct Expr {
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

// Forward declarations for all Stmt nodes
struct ExpressionStmt;
struct VarDeclStmt;
struct BlockStmt;

// Visitor for Statements
class StmtVisitor {
public:
    virtual std::any visit(const ExpressionStmt& stmt) = 0;
    virtual std::any visit(const VarDeclStmt& stmt) = 0;
    virtual std::any visit(const BlockStmt& stmt) = 0;
};


// Base class for all statement nodes
struct Stmt {
    virtual std::any accept(StmtVisitor& visitor) const = 0;
};


// --- Concrete Expression Nodes ---

struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

struct GroupingExpr : Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};


struct LiteralExpr : Expr {
    LiteralExpr(Literal value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Literal value;
};

struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};

struct VariableExpr : Expr {
    VariableExpr(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token name;
};


// --- Concrete Statement Nodes ---

struct VarDeclStmt : Stmt {
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> initializer;
};

struct ExpressionStmt : Stmt {
    ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct BlockStmt : Stmt {
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::vector<std::unique_ptr<Stmt>> statements;
};

} // namespace chtholly
