#pragma once

#include "Expr.h"
#include "../TypeInfo.h"
#include <any>
#include <memory>
#include <vector>
#include <optional>

struct ExpressionStmt;
struct PrintStmt;
struct LetStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct FunctionStmt;
struct ReturnStmt;
struct StructStmt;

class StmtVisitor {
public:
    virtual std::any visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual std::any visitPrintStmt(const PrintStmt& stmt) = 0;
    virtual std::any visitLetStmt(const LetStmt& stmt) = 0;
    virtual std::any visitBlockStmt(const BlockStmt& stmt) = 0;
    virtual std::any visitIfStmt(const IfStmt& stmt) = 0;
    virtual std::any visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual std::any visitFunctionStmt(const FunctionStmt& stmt) = 0;
    virtual std::any visitReturnStmt(const ReturnStmt& stmt) = 0;
    virtual std::any visitStructStmt(const StructStmt& stmt) = 0;
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
    std::optional<TypeInfo> type;
    std::unique_ptr<Expr> initializer;
    bool isMutable;

    LetStmt(Token name, std::optional<TypeInfo> type, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(name), type(std::move(type)), initializer(std::move(initializer)), isMutable(isMutable) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitLetStmt(*this);
    }
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitBlockStmt(*this);
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

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitWhileStmt(*this);
    }
};

struct FunctionStmt : Stmt {
    Token name;
    std::vector<std::pair<Token, TypeInfo>> params;
    std::vector<std::unique_ptr<Stmt>> body;
    std::optional<TypeInfo> returnType;

    FunctionStmt(Token name, std::vector<std::pair<Token, TypeInfo>> params, std::vector<std::unique_ptr<Stmt>> body, std::optional<TypeInfo> returnType)
        : name(name), params(std::move(params)), body(std::move(body)), returnType(std::move(returnType)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitFunctionStmt(*this);
    }
};

struct ReturnStmt : Stmt {
    Token keyword;
    std::unique_ptr<Expr> value;

    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitReturnStmt(*this);
    }
};

struct StructStmt : Stmt {
    Token name;
    std::vector<std::unique_ptr<LetStmt>> fields;

    StructStmt(Token name, std::vector<std::unique_ptr<LetStmt>> fields)
        : name(name), fields(std::move(fields)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitStructStmt(*this);
    }
};
