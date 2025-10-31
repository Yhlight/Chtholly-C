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
        result += " = " + value->to_string() + ";";
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
        result += " = " + value->to_string() + ";";
        return result;
    }
};
