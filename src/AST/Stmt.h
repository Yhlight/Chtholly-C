#ifndef CHTHOLLY_STMT_H
#define CHTHOLLY_STMT_H

#include "../Token.h"
#include "Expr.h"
#include <any>
#include <memory>
#include <vector>

class Stmt;

// Visitor interface for statements
class StmtVisitor {
public:
    virtual std::any visit(const class LetStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

class Stmt {
public:
    virtual std::any accept(StmtVisitor& visitor) const = 0;
    virtual ~Stmt() = default;
};

// Concrete statement classes
class LetStmt : public Stmt {
public:
    LetStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> initializer;
};

#endif // CHTHOLLY_STMT_H
