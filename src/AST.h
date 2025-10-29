#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <vector>
#include <memory>
#include "Token.h" // For TokenType

namespace Chtholly {

// Forward declarations
class Expression;
class Statement;
class BlockStatement;

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Base class for all expression nodes
class Expression : public ASTNode {};

// Base class for all statement nodes
class Statement : public ASTNode {};

// Represents a program, which is a collection of statements
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
};

// Expressions
class Identifier : public Expression {
public:
    Identifier(Token token) : token(std::move(token)) {}
    Token token; // Contains name and location
};

class IntegerLiteral : public Expression {
public:
    IntegerLiteral(Token token, long long value) : token(std::move(token)), value(value) {}
    Token token;
    long long value;
};

class DoubleLiteral : public Expression {
public:
    DoubleLiteral(Token token, double value) : token(std::move(token)), value(value) {}
    Token token;
    double value;
};

class StringLiteral : public Expression {
public:
    StringLiteral(Token token) : token(std::move(token)) {}
    Token token;
};

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
};

class FunctionCall : public Expression {
public:
    FunctionCall(Token token, std::unique_ptr<Expression> callee, std::vector<std::unique_ptr<Expression>> arguments)
        : token(std::move(token)), callee(std::move(callee)), arguments(std::move(arguments)) {}
    Token token; // The '(' token
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
};

class IfExpression : public Expression {
public:
    IfExpression(Token token, std::unique_ptr<Expression> condition, std::unique_ptr<BlockStatement> consequence, std::unique_ptr<BlockStatement> alternative)
        : token(std::move(token)), condition(std::move(condition)), consequence(std::move(consequence)), alternative(std::move(alternative)) {}
    Token token; // The 'if' token
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> consequence;
    std::unique_ptr<BlockStatement> alternative;
};


// Statements
class ExpressionStatement : public Statement {
public:
    ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    std::unique_ptr<Expression> expression;
};

class VariableDeclaration : public Statement {
public:
    VariableDeclaration(Token token, std::unique_ptr<Identifier> name, std::unique_ptr<Expression> value)
        : token(std::move(token)), name(std::move(name)), value(std::move(value)) {}
    Token token; // The 'let' or 'mut' token
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(Token token, std::unique_ptr<Expression> returnValue)
        : token(std::move(token)), returnValue(std::move(returnValue)) {}
    Token token; // The 'return' token
    std::unique_ptr<Expression> returnValue;
};

class BlockStatement : public Statement {
public:
    BlockStatement(Token token) : token(std::move(token)) {}
    Token token; // The '{' token
    std::vector<std::unique_ptr<Statement>> statements;
};

class FunctionDeclaration : public Statement {
public:
    FunctionDeclaration(Token token, std::unique_ptr<Identifier> name, std::vector<std::unique_ptr<Identifier>> parameters, std::unique_ptr<BlockStatement> body)
        : token(std::move(token)), name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {}
    Token token; // The 'func' token
    std::unique_ptr<Identifier> name;
    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<BlockStatement> body;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
