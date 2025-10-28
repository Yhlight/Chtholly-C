#pragma once

#include <string>
#include <memory>
#include <vector>

// Base class for all expression nodes.
class ExprAST {
public:
    virtual ~ExprAST() = default;
};

// Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double val_;

public:
    NumberExprAST(double val) : val_(val) {}
};

// Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
    std::string name_;

public:
    VariableExprAST(std::string name) : name_(std::move(name)) {}
};

// Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    char op_;
    std::unique_ptr<ExprAST> lhs_, rhs_;

public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs,
                  std::unique_ptr<ExprAST> rhs)
        : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
};

// Expression class for function calls.
class CallExprAST : public ExprAST {
    std::string callee_;
    std::vector<std::unique_ptr<ExprAST>> args_;

public:
    CallExprAST(std::string callee,
                std::vector<std::unique_ptr<ExprAST>> args)
        : callee_(std::move(callee)), args_(std::move(args)) {}
};

// This class represents the "prototype" for a function,
// which captures its name, and its argument names (thus implicitly the number
// of arguments the function takes).
class PrototypeAST {
    std::string name_;
    std::vector<std::string> args_;

public:
    PrototypeAST(std::string name, std::vector<std::string> args)
        : name_(std::move(name)), args_(std::move(args)) {}
};

// This class represents a function definition itself.
class FunctionAST {
    std::unique_ptr<PrototypeAST> proto_;
    std::unique_ptr<ExprAST> body_;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto,
                std::unique_ptr<ExprAST> body)
        : proto_(std::move(proto)), body_(std::move(body)) {}
};

// Expression class for variable declarations.
class VarDeclAST : public ExprAST {
    std::string name_;
    std::unique_ptr<ExprAST> init_;
    bool is_mutable_;

public:
    VarDeclAST(std::string name, std::unique_ptr<ExprAST> init, bool is_mutable)
        : name_(std::move(name)), init_(std::move(init)), is_mutable_(is_mutable) {}
};
