#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <any>
#include <memory>
#include <vector>
#include "Token.h"

// Forward declarations
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Logical;
struct Call;
struct Get;
struct Set;

// Visitor for expressions
struct ExprVisitor {
    virtual std::any visitBinaryExpr(const std::shared_ptr<Binary>& expr) = 0;
    virtual std::any visitGroupingExpr(const std::shared_ptr<Grouping>& expr) = 0;
    virtual std::any visitLiteralExpr(const std::shared_ptr<Literal>& expr) = 0;
    virtual std::any visitUnaryExpr(const std::shared_ptr<Unary>& expr) = 0;
    virtual std::any visitVariableExpr(const std::shared_ptr<Variable>& expr) = 0;
    virtual std::any visitAssignExpr(const std::shared_ptr<Assign>& expr) = 0;
    virtual std::any visitLogicalExpr(const std::shared_ptr<Logical>& expr) = 0;
    virtual std::any visitCallExpr(const std::shared_ptr<Call>& expr) = 0;
    virtual std::any visitGetExpr(const std::shared_ptr<Get>& expr) = 0;
    virtual std::any visitSetExpr(const std::shared_ptr<Set>& expr) = 0;
    virtual ~ExprVisitor() = default;
};

// Base class for expressions
struct Expr {
    virtual std::any accept(ExprVisitor& visitor) = 0;
    virtual ~Expr() = default;
};

// Forward declarations for statements
struct Expression;
struct Print;
struct Var;
struct Block;
struct If;
struct While;
struct Function;
struct Return;
struct Struct;

// Visitor for statements
struct StmtVisitor {
    virtual void visitExpressionStmt(const std::shared_ptr<Expression>& stmt) = 0;
    virtual void visitPrintStmt(const std::shared_ptr<Print>& stmt) = 0;
    virtual void visitVarStmt(const std::shared_ptr<Var>& stmt) = 0;
    virtual void visitBlockStmt(const std::shared_ptr<Block>& stmt) = 0;
    virtual void visitIfStmt(const std::shared_ptr<If>& stmt) = 0;
    virtual void visitWhileStmt(const std::shared_ptr<While>& stmt) = 0;
    virtual void visitFunctionStmt(const std::shared_ptr<Function>& stmt) = 0;
    virtual void visitReturnStmt(const std::shared_ptr<Return>& stmt) = 0;
    virtual void visitStructStmt(const std::shared_ptr<Struct>& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

// Base class for statements
struct Stmt {
    virtual void accept(StmtVisitor& visitor) = 0;
    virtual ~Stmt() = default;
};

// Concrete expression nodes
struct Binary : Expr, public std::enable_shared_from_this<Binary> {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
    std::shared_ptr<Expr> expression;

    explicit Grouping(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitGroupingExpr(shared_from_this());
    }
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
    std::any value;

    explicit Literal(std::any value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLiteralExpr(shared_from_this());
    }
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
    Token op;
    std::shared_ptr<Expr> right;

    Unary(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitUnaryExpr(shared_from_this());
    }
};

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
    Token name;

    explicit Variable(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitVariableExpr(shared_from_this());
    }
};

struct Assign : Expr, public std::enable_shared_from_this<Assign> {
    Token name;
    std::shared_ptr<Expr> value;

    Assign(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitAssignExpr(shared_from_this());
    }
};

struct Logical : Expr, public std::enable_shared_from_this<Logical> {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLogicalExpr(shared_from_this());
    }
};

struct Call : Expr, public std::enable_shared_from_this<Call> {
    std::shared_ptr<Expr> callee;
    Token paren; // for error reporting
    std::vector<std::shared_ptr<Expr>> arguments;

    Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitCallExpr(shared_from_this());
    }
};

struct Get : Expr, public std::enable_shared_from_this<Get> {
    std::shared_ptr<Expr> object;
    Token name;

    Get(std::shared_ptr<Expr> object, Token name)
        : object(std::move(object)), name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitGetExpr(shared_from_this());
    }
};

struct Set : Expr, public std::enable_shared_from_this<Set> {
    std::shared_ptr<Expr> object;
    Token name;
    std::shared_ptr<Expr> value;

    Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
        : object(std::move(object)), name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitSetExpr(shared_from_this());
    }
};


// Concrete statement nodes
struct Expression : Stmt, public std::enable_shared_from_this<Expression> {
    std::shared_ptr<Expr> expression;

    explicit Expression(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitExpressionStmt(shared_from_this());
    }
};

struct Print : Stmt, public std::enable_shared_from_this<Print> {
    std::shared_ptr<Expr> expression;

    explicit Print(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitPrintStmt(shared_from_this());
    }
};

struct Var : Stmt, public std::enable_shared_from_this<Var> {
    Token name;
    std::shared_ptr<Expr> initializer;
    Token keyword; // 'let' or 'mut'

    Var(Token name, std::shared_ptr<Expr> initializer, Token keyword)
        : name(std::move(name)), initializer(std::move(initializer)), keyword(std::move(keyword)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitVarStmt(shared_from_this());
    }
};

struct Block : Stmt, public std::enable_shared_from_this<Block> {
    std::vector<std::shared_ptr<Stmt>> statements;

    explicit Block(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitBlockStmt(shared_from_this());
    }
};

struct If : Stmt, public std::enable_shared_from_this<If> {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;

    If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitIfStmt(shared_from_this());
    }
};

struct While : Stmt, public std::enable_shared_from_this<While> {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;

    While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitWhileStmt(shared_from_this());
    }
};

struct Function : Stmt, public std::enable_shared_from_this<Function> {
    Token name;
    std::vector<Token> params;
    std::vector<std::shared_ptr<Stmt>> body;

    Function(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitFunctionStmt(shared_from_this());
    }
};

struct Return : Stmt, public std::enable_shared_from_this<Return> {
    Token keyword;
    std::shared_ptr<Expr> value;

    Return(Token keyword, std::shared_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitReturnStmt(shared_from_this());
    }
};

struct Struct : Stmt, public std::enable_shared_from_this<Struct> {
    Token name;
    std::vector<std::shared_ptr<Var>> fields;
    std::vector<std::shared_ptr<Function>> methods;

    Struct(Token name, std::vector<std::shared_ptr<Var>> fields, std::vector<std::shared_ptr<Function>> methods)
        : name(std::move(name)), fields(std::move(fields)), methods(std::move(methods)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitStructStmt(shared_from_this());
    }
};

#endif //CHTHOLLY_AST_H
