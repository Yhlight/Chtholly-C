#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "Token.h"
#include <vector>
#include <memory>

struct Type {
    Token name;
};

// Forward declarations for visitor pattern
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Call;
struct Get;
struct Set;
struct Self;

// Visitor for expressions
template<typename R>
struct ExprVisitor {
    virtual R visitBinaryExpr(const std::shared_ptr<Binary>& expr) = 0;
    virtual R visitGroupingExpr(const std::shared_ptr<Grouping>& expr) = 0;
    virtual R visitLiteralExpr(const std::shared_ptr<Literal>& expr) = 0;
    virtual R visitUnaryExpr(const std::shared_ptr<Unary>& expr) = 0;
    virtual R visitVariableExpr(const std::shared_ptr<Variable>& expr) = 0;
    virtual R visitAssignExpr(const std::shared_ptr<Assign>& expr) = 0;
    virtual R visitCallExpr(const std::shared_ptr<Call>& expr) = 0;
    virtual R visitGetExpr(const std::shared_ptr<Get>& expr) = 0;
    virtual R visitSetExpr(const std::shared_ptr<Set>& expr) = 0;
    virtual R visitSelfExpr(const std::shared_ptr<Self>& expr) = 0;
};

// Base class for all expressions
struct Expr {
    virtual ~Expr() = default;
    virtual std::string accept(ExprVisitor<std::string>& visitor) = 0;
};

// Concrete expression nodes
struct Binary : Expr, public std::enable_shared_from_this<Binary> {
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(left), op(op), right(right) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }

    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
    Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitGroupingExpr(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
    Literal(std::string value, TokenType type) : value(value), type(type) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitLiteralExpr(shared_from_this());
    }

    std::string value;
    TokenType type;
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
    Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitUnaryExpr(shared_from_this());
    }

    Token op;
    std::shared_ptr<Expr> right;
};

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
    Variable(Token name) : name(name) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitVariableExpr(shared_from_this());
    }

    Token name;
};

struct Assign : Expr, public std::enable_shared_from_this<Assign> {
    Assign(Token name, std::shared_ptr<Expr> value) : name(name), value(value) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitAssignExpr(shared_from_this());
    }

    Token name;
    std::shared_ptr<Expr> value;
};

struct Call : Expr, public std::enable_shared_from_this<Call> {
    Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(callee), paren(paren), arguments(arguments) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitCallExpr(shared_from_this());
    }

    std::shared_ptr<Expr> callee;
    Token paren;
    std::vector<std::shared_ptr<Expr>> arguments;
};

struct Get : Expr, public std::enable_shared_from_this<Get> {
    Get(std::shared_ptr<Expr> object, Token name)
        : object(object), name(name) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitGetExpr(shared_from_this());
    }

    std::shared_ptr<Expr> object;
    Token name;
};

struct Set : Expr, public std::enable_shared_from_this<Set> {
    Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
        : object(object), name(name), value(value) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitSetExpr(shared_from_this());
    }

    std::shared_ptr<Expr> object;
    Token name;
    std::shared_ptr<Expr> value;
};

struct Self : Expr, public std::enable_shared_from_this<Self> {
    Self(Token keyword) : keyword(keyword) {}

    std::string accept(ExprVisitor<std::string>& visitor) override {
        return visitor.visitSelfExpr(shared_from_this());
    }

    Token keyword;
};

// Forward declarations for statement visitor
struct Expression;
struct Var;
struct Block;
struct If;
struct While;
struct Func;
struct Return;
struct For;
struct Switch;
struct Break;
struct Fallthrough;
struct Struct;

// Visitor for statements
template<typename R>
struct StmtVisitor {
    virtual R visitExpressionStmt(const std::shared_ptr<Expression>& stmt) = 0;
    virtual R visitVarStmt(const std::shared_ptr<Var>& stmt) = 0;
    virtual R visitBlockStmt(const std::shared_ptr<Block>& stmt) = 0;
    virtual R visitIfStmt(const std::shared_ptr<If>& stmt) = 0;
    virtual R visitWhileStmt(const std::shared_ptr<While>& stmt) = 0;
    virtual R visitFuncStmt(const std::shared_ptr<Func>& stmt) = 0;
    virtual R visitReturnStmt(const std::shared_ptr<Return>& stmt) = 0;
    virtual R visitForStmt(const std::shared_ptr<For>& stmt) = 0;
    virtual R visitSwitchStmt(const std::shared_ptr<Switch>& stmt) = 0;
    virtual R visitBreakStmt(const std::shared_ptr<Break>& stmt) = 0;
    virtual R visitFallthroughStmt(const std::shared_ptr<Fallthrough>& stmt) = 0;
    virtual R visitStructStmt(const std::shared_ptr<Struct>& stmt) = 0;
};

// Base class for all statements
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::string accept(StmtVisitor<std::string>& visitor) = 0;
};

// Concrete statement nodes
struct Expression : Stmt, public std::enable_shared_from_this<Expression> {
    Expression(std::shared_ptr<Expr> expression) : expression(expression) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitExpressionStmt(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct Var : Stmt, public std::enable_shared_from_this<Var> {
    Var(Token name, std::shared_ptr<Type> type, std::shared_ptr<Expr> initializer, bool is_mutable)
        : name(name), type(type), initializer(initializer), is_mutable(is_mutable) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitVarStmt(shared_from_this());
    }

    Token name;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expr> initializer;
    bool is_mutable;
};

struct Block : Stmt, public std::enable_shared_from_this<Block> {
    Block(std::vector<std::shared_ptr<Stmt>> statements) : statements(statements) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitBlockStmt(shared_from_this());
    }

    std::vector<std::shared_ptr<Stmt>> statements;
};

struct If : Stmt, public std::enable_shared_from_this<If> {
    If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitIfStmt(shared_from_this());
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
};

struct While : Stmt, public std::enable_shared_from_this<While> {
    While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(condition), body(body) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitWhileStmt(shared_from_this());
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
};

struct Parameter {
    Token name;
    std::shared_ptr<Type> type;
};

struct Func : Stmt, public std::enable_shared_from_this<Func> {
    Func(Token name, std::vector<Parameter> params, std::vector<std::shared_ptr<Stmt>> body, std::shared_ptr<Type> returnType)
        : name(name), params(params), body(body), returnType(returnType) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitFuncStmt(shared_from_this());
    }

    Token name;
    std::vector<Parameter> params;
    std::vector<std::shared_ptr<Stmt>> body;
    std::shared_ptr<Type> returnType;
};

struct Return : Stmt, public std::enable_shared_from_this<Return> {
    Return(Token keyword, std::shared_ptr<Expr> value)
        : keyword(keyword), value(value) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitReturnStmt(shared_from_this());
    }

    Token keyword;
    std::shared_ptr<Expr> value;
};

struct For : Stmt, public std::enable_shared_from_this<For> {
    For(std::shared_ptr<Stmt> initializer, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> increment, std::shared_ptr<Stmt> body)
        : initializer(initializer), condition(condition), increment(increment), body(body) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitForStmt(shared_from_this());
    }

    std::shared_ptr<Stmt> initializer;
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> increment;
    std::shared_ptr<Stmt> body;
};

struct Case : Stmt, public std::enable_shared_from_this<Case> {
    Case(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(condition), body(body) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        // Not directly visited, handled by SwitchStmt
        return "";
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
};

struct Switch : Stmt, public std::enable_shared_from_this<Switch> {
    Switch(std::shared_ptr<Expr> condition, std::vector<std::shared_ptr<Case>> cases)
        : condition(condition), cases(cases) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitSwitchStmt(shared_from_this());
    }

    std::shared_ptr<Expr> condition;
    std::vector<std::shared_ptr<Case>> cases;
};

struct Break : Stmt, public std::enable_shared_from_this<Break> {
    Break(Token keyword) : keyword(keyword) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitBreakStmt(shared_from_this());
    }

    Token keyword;
};

struct Fallthrough : Stmt, public std::enable_shared_from_this<Fallthrough> {
    Fallthrough(Token keyword) : keyword(keyword) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitFallthroughStmt(shared_from_this());
    }

    Token keyword;
};

struct Struct : Stmt, public std::enable_shared_from_this<Struct> {
    Struct(Token name, std::vector<std::shared_ptr<Var>> fields, std::vector<std::shared_ptr<Func>> methods)
        : name(name), fields(fields), methods(methods) {}

    std::string accept(StmtVisitor<std::string>& visitor) override {
        return visitor.visitStructStmt(shared_from_this());
    }

    Token name;
    std::vector<std::shared_ptr<Var>> fields;
    std::vector<std::shared_ptr<Func>> methods;
};

#endif //CHTHOLLY_AST_H
