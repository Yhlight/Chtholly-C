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

// Base class for all type nodes
struct Type : public Node {
    //
};

class TypeName : public Type {
public:
    TypeName(Token token, std::string name)
        : token(std::move(token)), name(std::move(name)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::string name;
    std::vector<std::unique_ptr<Type>> templateArgs;
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
    std::unique_ptr<Type> type;
};

// Represents a 'let' statement (e.g., let x = 5;)
class LetStatement : public Statement {
public:
    LetStatement(Token token) : token(std::move(token)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The 'let' or 'mut' token
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;
    std::unique_ptr<Type> type;
    bool isMutable;
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

class StringLiteral : public Expression {
public:
    StringLiteral(Token token, std::string value)
        : token(std::move(token)), value(std::move(value)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::string value;
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
    std::vector<std::unique_ptr<Identifier>> templateParams;
    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<BlockStatement> body;
    std::unique_ptr<Type> returnType;
};

class CallExpression : public Expression {
public:
    CallExpression(Token token, std::unique_ptr<Expression> function)
        : token(std::move(token)), function(std::move(function)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The '(' token
    std::unique_ptr<Expression> function; // Identifier or FunctionLiteral
    std::vector<std::unique_ptr<Type>> templateArgs;
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

class Member : public Node {
public:
    Member(Token token, bool isPublic)
        : token(std::move(token)), isPublic(isPublic) {}

    Token token;
    bool isPublic;
};

class Field : public Member {
public:
    Field(Token token, bool isPublic, std::unique_ptr<Identifier> name, std::unique_ptr<Type> type)
        : Member(std::move(token), isPublic), name(std::move(name)), type(std::move(type)) {}

    std::string tokenLiteral() const override { return name->tokenLiteral(); }

    std::unique_ptr<Identifier> name;
    std::unique_ptr<Type> type;
};

class Method : public Member {
public:
    Method(Token token, bool isPublic, std::unique_ptr<Identifier> name, std::unique_ptr<FunctionLiteral> function)
        : Member(std::move(token), isPublic), name(std::move(name)), function(std::move(function)) {}

    std::string tokenLiteral() const override { return name->tokenLiteral(); }

    std::unique_ptr<Identifier> name;
    std::unique_ptr<FunctionLiteral> function;
};

class StructStatement : public Statement {
public:
    StructStatement(Token token) : token(std::move(token)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token; // The 'struct' token
    std::unique_ptr<Identifier> name;
    std::vector<std::unique_ptr<Identifier>> templateParams;
    std::vector<std::unique_ptr<Member>> members;
};

class GenericInstantiation : public Expression {
public:
    GenericInstantiation(Token token, std::unique_ptr<Expression> base, std::vector<std::unique_ptr<Type>> args)
        : token(std::move(token)), base(std::move(base)), arguments(std::move(args)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::unique_ptr<Expression> base;
    std::vector<std::unique_ptr<Type>> arguments;
};

class StructLiteral : public Expression {
public:
    StructLiteral(Token token, std::unique_ptr<Expression> name)
        : token(std::move(token)), structName(std::move(name)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::unique_ptr<Expression> structName;
    std::vector<std::pair<std::unique_ptr<Identifier>, std::unique_ptr<Expression>>> fields;
};

class MemberAccessExpression : public Expression {
public:
    MemberAccessExpression(Token token, std::unique_ptr<Expression> object, std::unique_ptr<Identifier> member)
        : token(std::move(token)), object(std::move(object)), member(std::move(member)) {}

    std::string tokenLiteral() const override { return token.literal; }

    Token token;
    std::unique_ptr<Expression> object;
    std::unique_ptr<Identifier> member;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
