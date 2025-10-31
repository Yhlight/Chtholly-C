#pragma once

#include <memory>
#include <string>
#include <vector>

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

// Represents a let statement
struct LetStatement : public Statement {
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    std::string to_string() const override {
        return "let " + name->to_string() + " = " + value->to_string() + ";";
    }
};
