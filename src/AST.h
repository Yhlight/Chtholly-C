#pragma once

#include <memory>
#include <string>
#include <vector>
#include <utility>

// Base class for all nodes in the AST
struct Node {
    virtual ~Node() = default;
    virtual std::string to_string() const = 0;
};

// Base class for all expressions
struct Expression : public Node {};

// Base class for all statements
struct Statement : public Node {};

// Represents a program, which is a collection of statements
struct Program : public Node {
    std::vector<std::unique_ptr<Statement>> statements;

    std::string to_string() const override {
        std::string result;
        for (const auto& stmt : statements) {
            result += stmt->to_string();
        }
        return result;
    }
};

// Represents an identifier
struct Identifier : public Expression {
    std::string value;

    explicit Identifier(std::string value) : value(std::move(value)) {}

    std::string to_string() const override {
        return value;
    }
};

// Represents a type annotation
struct Type : public Node {
    std::string name;

    explicit Type(std::string name) : name(std::move(name)) {}

    std::string to_string() const override {
        return name;
    }
};

// Represents an integer literal
struct IntegerLiteral : public Expression {
    int64_t value;

    std::string to_string() const override {
        return std::to_string(value);
    }
};

// Represents a let statement
struct LetStatement : public Statement {
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> value;

    std::string to_string() const override {
        std::string result = "let " + name->to_string();
        if (type) {
            result += " : " + type->to_string();
        }
        if (value) {
            result += " = " + value->to_string();
        }
        result += ";";
        return result;
    }
};

// Represents a mut statement
struct MutStatement : public Statement {
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> value;

    std::string to_string() const override {
        std::string result = "mut " + name->to_string();
        if (type) {
            result += " : " + type->to_string();
        }
        if (value) {
            result += " = " + value->to_string();
        }
        result += ";";
        return result;
    }
};

// Represents a block of statements
struct BlockStatement : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;

    std::string to_string() const override {
        std::string result = "{ ";
        for (const auto& stmt : statements) {
            if (stmt) {
                result += stmt->to_string();
            }
        }
        result += " }";
        return result;
    }
};

// Represents a function declaration
struct FunctionStatement : public Statement {
    std::unique_ptr<Identifier> name;
    std::vector<std::pair<std::unique_ptr<Identifier>, std::unique_ptr<Type>>> parameters;
    std::unique_ptr<Type> return_type;
    std::unique_ptr<BlockStatement> body;

    std::string to_string() const override {
        std::string result = "func " + name->to_string() + "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            result += parameters[i].first->to_string() + " : " + parameters[i].second->to_string();
            if (i < parameters.size() - 1) {
                result += ", ";
            }
        }
        result += ")";
        if (return_type) {
            result += " -> " + return_type->to_string();
        }
        if (body) {
            result += " " + body->to_string();
        }
        return result;
    }
};
