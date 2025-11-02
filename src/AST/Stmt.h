#pragma once

#include "Expr.h"
#include <any>
#include <memory>
#include <vector>

struct ExpressionStmt;
struct PrintStmt;
struct LetStmt;

class StmtVisitor {
public:
    virtual std::any visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual std::any visitPrintStmt(const PrintStmt& stmt) = 0;
    virtual std::any visitLetStmt(const LetStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

class Stmt {
public:
    virtual std::any accept(StmtVisitor& visitor) const = 0;
    virtual ~Stmt() = default;
};

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
        : name(name), initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitLetStmt(*this);
    }
};
