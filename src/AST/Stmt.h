#pragma once

#include "Expr.h"
#include <memory>
#include <vector>
#include <any>

// Forward declarations
struct BlockStmt;
struct ExpressionStmt;
struct IfStmt;
struct PrintStmt;
struct LetStmt;
struct WhileStmt;
struct SwitchStmt;
struct CaseStmt;
struct BreakStmt;
struct FallthroughStmt;

// Visitor interface
struct StmtVisitor {
    virtual std::any visitBlockStmt(const BlockStmt& stmt) = 0;
    virtual std::any visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual std::any visitIfStmt(const IfStmt& stmt) = 0;
    virtual std::any visitPrintStmt(const PrintStmt& stmt) = 0;
    virtual std::any visitLetStmt(const LetStmt& stmt) = 0;
    virtual std::any visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual std::any visitSwitchStmt(const SwitchStmt& stmt) = 0;
    virtual std::any visitCaseStmt(const CaseStmt& stmt) = 0;
    virtual std::any visitBreakStmt(const BreakStmt& stmt) = 0;
    virtual std::any visitFallthroughStmt(const FallthroughStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor& visitor) const = 0;
};

// Concrete statement classes
struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitBlockStmt(*this);
    }
};

struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;

    explicit ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitExpressionStmt(*this);
    }
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitIfStmt(*this);
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

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitWhileStmt(*this);
    }
};

struct CaseStmt : Stmt {
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> body;

	CaseStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
		: condition(std::move(condition)), body(std::move(body)) {}

	std::any accept(StmtVisitor& visitor) const override {
		return visitor.visitCaseStmt(*this);
	}
};

struct SwitchStmt : Stmt {
    std::unique_ptr<Expr> expression;
    std::vector<std::unique_ptr<CaseStmt>> cases;

    SwitchStmt(std::unique_ptr<Expr> expression, std::vector<std::unique_ptr<CaseStmt>> cases)
        : expression(std::move(expression)), cases(std::move(cases)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitSwitchStmt(*this);
    }
};

struct BreakStmt : Stmt {
    Token keyword;
    BreakStmt(Token keyword) : keyword(std::move(keyword)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitBreakStmt(*this);
    }
};

struct FallthroughStmt : Stmt {
    Token keyword;
    FallthroughStmt(Token keyword) : keyword(std::move(keyword)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitFallthroughStmt(*this);
    }
};
