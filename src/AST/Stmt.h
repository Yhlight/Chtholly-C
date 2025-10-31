#ifndef CHTHOLLY_STMT_H
#define CHTHOLLY_STMT_H

#include "../Token.h"
#include "../Type.h"
#include "Expr.h"
#include <any>
#include <memory>
#include <vector>

class Stmt;

// Visitor interface for statements
class StmtVisitor {
public:
    virtual void visit(const class LetStmt& stmt) = 0;
    virtual void visit(const class FuncStmt& stmt) = 0;
    virtual void visit(const class BlockStmt& stmt) = 0;
    virtual void visit(const class ReturnStmt& stmt) = 0;
    virtual void visit(const class ExprStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

class Stmt {
public:
    virtual void accept(StmtVisitor& visitor) const = 0;
    virtual ~Stmt() = default;
};

// Concrete statement classes
class LetStmt : public Stmt {
public:
    LetStmt(Token name, std::unique_ptr<Type> type, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(std::move(name)), type(std::move(type)), initializer(std::move(initializer)), isMutable(isMutable) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<Type> type;
    const std::unique_ptr<Expr> initializer;
    const bool isMutable;
};

class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::vector<std::unique_ptr<Stmt>> statements;
};

class FuncStmt : public Stmt {
public:
    struct Parameter {
        Token name;
        std::unique_ptr<Type> type;
        bool isMutable;
    };

    FuncStmt(Token name, std::vector<Parameter> params, std::unique_ptr<Type> returnType, std::unique_ptr<BlockStmt> body)
        : name(std::move(name)), params(std::move(params)), returnType(std::move(returnType)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::vector<Parameter> params;
    const std::unique_ptr<Type> returnType;
    const std::unique_ptr<BlockStmt> body;
};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token keyword;
    const std::unique_ptr<Expr> value;
};

class ExprStmt : public Stmt {
public:
    ExprStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};

#endif // CHTHOLLY_STMT_H
