#ifndef CHTHOLLY_STMT_H
#define CHTHOLLY_STMT_H

#include <memory>
#include <vector>
#include "../Token.h"
#include "Expr.h"
#include "../Type.h"

// Forward declarations
class Block;
class ExpressionStmt;
class LetStmt;
class PrintStmt;

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitBlockStmt(const Block& stmt) = 0;
    virtual void visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual void visitLetStmt(const LetStmt& stmt) = 0;
    virtual void visitPrintStmt(const PrintStmt& stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

class Block : public Stmt {
public:
    Block(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitBlockStmt(*this);
    }

    const std::vector<std::unique_ptr<Stmt>> statements;
};

class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitExpressionStmt(*this);
    }

    const std::unique_ptr<Expr> expression;
};

class LetStmt : public Stmt {
public:
    LetStmt(Token name, std::unique_ptr<Type> type, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(name), type(std::move(type)), initializer(std::move(initializer)), isMutable(isMutable) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitLetStmt(*this);
    }

    const Token name;
    const std::unique_ptr<Type> type;
    const std::unique_ptr<Expr> initializer;
    const bool isMutable;
};

// A temporary statement for printing expressions, for testing purposes.
class PrintStmt : public Stmt {
public:
    PrintStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitPrintStmt(*this);
    }

    const std::unique_ptr<Expr> expression;
};

#endif // CHTHOLLY_STMT_H
