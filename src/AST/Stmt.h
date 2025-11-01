#pragma once

#include "Expr.h"
#include <memory>
#include <vector>
#include <any>

// Forward declarations
struct ExpressionStmt;
struct PrintStmt;
struct LetStmt;

// Visitor interface
struct StmtVisitor {
    virtual std::any visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual std::any visitPrintStmt(const PrintStmt& stmt) = 0;
    virtual std::any visitLetStmt(const LetStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor& visitor) const = 0;
};

// Concrete statement classes
struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;

    explicit ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitExpressionStmt(*this);
    }
};

struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expression;

    explicit PrintStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitPrintStmt(*this);
    }
};

struct LetStmt : Stmt {
    Token name;
    std::unique_ptr<Expr> initializer;

    LetStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitLetStmt(*this);
    }
};
