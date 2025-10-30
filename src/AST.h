#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "Token.h"
#include <memory>
#include <string>
#include <vector>

namespace Chtholly {

// Base class for all nodes in the AST
struct Node {
    virtual ~Node() = default;
    virtual std::string tokenLiteral() const = 0;
};

// Base class for all expression nodes
struct Expression : public Node {
    //
};

// Base class for all statement nodes
struct Statement : public Node {
    //
};

// The root node of the AST
class Program : public Node {
public:
    std::string tokenLiteral() const override {
        if (!statements.empty()) {
            return statements[0]->tokenLiteral();
        }
        return "";
    }

    std::vector<std::unique_ptr<Statement>> statements;
};

// Represents an identifier
class Identifier : public Expression {
public:
    Identifier(Token token, std::string value)
        : token(std::move(token)), value(std::move(value)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::string value;
};

// Represents a 'let' statement (e.g., let x = 5;)
class LetStatement : public Statement {
public:
    LetStatement(Token token) : token(std::move(token)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The 'let' token
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Token token) : token(std::move(token)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::unique_ptr<Expression> expression;
};

class IntegerLiteral : public Expression {
public:
    IntegerLiteral(Token token, int64_t value)
        : token(std::move(token)), value(value) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    int64_t value;
};

class Boolean : public Expression {
public:
    Boolean(Token token, bool value)
        : token(std::move(token)), value(value) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    bool value;
};

class PrefixExpression : public Expression {
public:
    PrefixExpression(Token token, std::string op)
        : token(std::move(token)), op(std::move(op)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::string op;
    std::unique_ptr<Expression> right;
};

class InfixExpression : public Expression {
public:
    InfixExpression(Token token, std::string op, std::unique_ptr<Expression> left)
        : token(std::move(token)), op(std::move(op)), left(std::move(left)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

class BlockStatement : public Statement {
public:
    BlockStatement(Token token) : token(std::move(token)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The '{' token
    std::vector<std::unique_ptr<Statement>> statements;
};

class IfExpression : public Expression {
public:
    IfExpression(Token token) : token(std::move(token)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The 'if' token
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> consequence;
    std::unique_ptr<BlockStatement> alternative;
};

class FunctionLiteral : public Expression {
public:
    FunctionLiteral(Token token) : token(std::move(token)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The 'func' token
    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<BlockStatement> body;
};

class CallExpression : public Expression {
public:
    CallExpression(Token token, std::unique_ptr<Expression> function)
        : token(std::move(token)), function(std::move(function)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The '(' token
    std::unique_ptr<Expression> function; // Identifier or FunctionLiteral
    std::vector<std::unique_ptr<Expression>> arguments;
};

// Represents a 'return' statement (e.g., return 5;)
class ReturnStatement : public Statement {
public:
    ReturnStatement(Token token) : token(std::move(token)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The 'return' token
    std::unique_ptr<Expression> returnValue;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
