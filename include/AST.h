#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "Token.h"
#include <vector>
#include <memory>

// Forward declarations
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct VariableExpr;
struct AssignmentExpr;
struct CallExpr;
struct GetExpr;
struct SetExpr;
struct SelfExpr;

// Visitor interface for expressions
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const BinaryExpr& expr) = 0;
    virtual void visit(const GroupingExpr& expr) = 0;
    virtual void visit(const LiteralExpr& expr) = 0;
    virtual void visit(const UnaryExpr& expr) = 0;
    virtual void visit(const VariableExpr& expr) = 0;
    virtual void visit(const AssignmentExpr& expr) = 0;
    virtual void visit(const CallExpr& expr) = 0;
    virtual void visit(const GetExpr& expr) = 0;
    virtual void visit(const SetExpr& expr) = 0;
    virtual void visit(const SelfExpr& expr) = 0;
};

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

// Binary expressions
struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Get expressions
struct GetExpr : Expr {
    std::unique_ptr<Expr> object;
    Token name;

    GetExpr(std::unique_ptr<Expr> object, Token name)
        : object(std::move(object)), name(name) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Set expressions
struct SetExpr : Expr {
    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;

    SetExpr(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value)
        : object(std::move(object)), name(name), value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Self expressions
struct SelfExpr : Expr {
    Token keyword;

    SelfExpr(Token keyword) : keyword(keyword) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Grouping expressions
struct GroupingExpr : Expr {
    std::unique_ptr<Expr> expression;

    GroupingExpr(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Literal expressions
struct LiteralExpr : Expr {
    Token value;

    LiteralExpr(Token value) : value(value) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Unary expressions
struct UnaryExpr : Expr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token op, std::unique_ptr<Expr> right) : op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Variable expressions
struct VariableExpr : Expr {
    Token name;

    VariableExpr(Token name) : name(name) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Assignment expressions
struct AssignmentExpr : Expr {
    Token name;
    std::unique_ptr<Expr> value;

    AssignmentExpr(Token name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Call expressions
struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;

    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }
};


// Forward declarations for statements
struct BlockStmt;
struct ExpressionStmt;
struct FunctionStmt;
struct IfStmt;
struct ReturnStmt;
struct VarDeclStmt;
struct WhileStmt;
struct StructStmt;

// Visitor interface for statements
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(const BlockStmt& stmt) = 0;
    virtual void visit(const ExpressionStmt& stmt) = 0;
    virtual void visit(const FunctionStmt& stmt) = 0;
    virtual void visit(const IfStmt& stmt) = 0;
    virtual void visit(const ReturnStmt& stmt) = 0;
    virtual void visit(const VarDeclStmt& stmt) = 0;
    virtual void visit(const WhileStmt& stmt) = 0;
    virtual void visit(const StructStmt& stmt) = 0;
};


// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

// Block statements
struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Struct statements
struct StructStmt : Stmt {
    Token name;
    std::vector<std::unique_ptr<VarDeclStmt>> fields;
    std::vector<std::unique_ptr<FunctionStmt>> methods;

    StructStmt(Token name, std::vector<std::unique_ptr<VarDeclStmt>> fields, std::vector<std::unique_ptr<FunctionStmt>> methods)
        : name(name), fields(std::move(fields)), methods(std::move(methods)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Expression statements
struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;

    ExpressionStmt(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Function statements
struct FunctionStmt : Stmt {
    Token name;
    std::vector<Token> params;
    std::unique_ptr<BlockStmt> body;

    FunctionStmt(Token name, std::vector<Token> params, std::unique_ptr<BlockStmt> body)
        : name(name), params(std::move(params)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// If statements
struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Return statements
struct ReturnStmt : Stmt {
    Token keyword;
    std::unique_ptr<Expr> value;

    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Variable declaration statements
struct VarDeclStmt : Stmt {
    Token name;
    Token type;
    std::unique_ptr<Expr> initializer;
    bool isMutable;

    VarDeclStmt(Token name, Token type, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(name), type(type), initializer(std::move(initializer)), isMutable(isMutable) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// While statements
struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

#endif // CHTHOLLY_AST_H
