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
class FuncStmt;
class IfStmt;
class LetStmt;
class PrintStmt;
class ReturnStmt;
class WhileStmt;

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitBlockStmt(const Block& stmt) = 0;
    virtual void visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual void visitFuncStmt(const FuncStmt& stmt) = 0;
    virtual void visitIfStmt(const IfStmt& stmt) = 0;
    virtual void visitLetStmt(const LetStmt& stmt) = 0;
    virtual void visitPrintStmt(const PrintStmt& stmt) = 0;
    virtual void visitReturnStmt(const ReturnStmt& stmt) = 0;
    virtual void visitWhileStmt(const WhileStmt& stmt) = 0;
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

class FuncStmt : public Stmt {
public:
    FuncStmt(Token name, std::vector<std::pair<Token, std::unique_ptr<Type>>> params,
             std::unique_ptr<Type> returnType, std::unique_ptr<Stmt> body)
        : name(name), params(std::move(params)), returnType(std::move(returnType)),
          body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitFuncStmt(*this);
    }

    const Token name;
    const std::vector<std::pair<Token, std::unique_ptr<Type>>> params;
    const std::unique_ptr<Type> returnType;
    const std::unique_ptr<Stmt> body;
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

class ReturnStmt : public Stmt {
public:
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitReturnStmt(*this);
    }

    const Token keyword;
    const std::unique_ptr<Expr> value;
};

class IfStmt : public Stmt {
public:
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitIfStmt(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> thenBranch;
    const std::unique_ptr<Stmt> elseBranch;
};

class WhileStmt : public Stmt {
public:
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitWhileStmt(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> body;
};

#endif // CHTHOLLY_STMT_H
