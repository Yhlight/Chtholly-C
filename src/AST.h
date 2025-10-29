#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Type.h"

// Base class for all expression nodes.
class ExprAST {
public:
    virtual ~ExprAST() = default;
    std::shared_ptr<Type> type;
};

// Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double val_;

public:
    NumberExprAST(double val) : val_(val) {}
    double get_val() const { return val_; }
};

// Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
    std::string name_;

public:
    VariableExprAST(std::string name) : name_(std::move(name)) {}
    const std::string& get_name() const { return name_; }
};

// Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    char op_;
    std::unique_ptr<ExprAST> lhs_, rhs_;
    bool is_assignment_;

public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs,
                  std::unique_ptr<ExprAST> rhs, bool is_assignment = false)
        : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)), is_assignment_(is_assignment) {}

    char get_op() const { return op_; }
    ExprAST* get_lhs() const { return lhs_.get(); }
    ExprAST* get_rhs() const { return rhs_.get(); }
    bool is_assignment() const { return is_assignment_; }
};

// Expression class for function calls.
class CallExprAST : public ExprAST {
    std::string callee_;
    std::vector<std::unique_ptr<ExprAST>> args_;

public:
    CallExprAST(std::string callee,
                std::vector<std::unique_ptr<ExprAST>> args)
        : callee_(std::move(callee)), args_(std::move(args)) {}

    const std::string& get_callee() const { return callee_; }
    const std::vector<std::unique_ptr<ExprAST>>& get_args() const { return args_; }
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

    const std::string& get_name() const { return name_; }
    const std::vector<std::string>& get_args() const { return args_; }
};

// This class represents a function definition itself.
class FunctionAST {
    std::unique_ptr<PrototypeAST> proto_;
    std::unique_ptr<ExprAST> body_;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto,
                std::unique_ptr<ExprAST> body)
        : proto_(std::move(proto)), body_(std::move(body)) {}

    PrototypeAST* get_proto() const { return proto_.get(); }
    ExprAST* get_body() const { return body_.get(); }
};

// Expression class for variable declarations.
class VarDeclAST : public ExprAST {
    std::string name_;
    std::unique_ptr<ExprAST> init_;
    bool is_mutable_;

public:
    VarDeclAST(std::string name, std::unique_ptr<ExprAST> init, bool is_mutable)
        : name_(std::move(name)), init_(std::move(init)), is_mutable_(is_mutable) {}

    const std::string& get_name() const { return name_; }
    ExprAST* get_init() const { return init_.get(); }
    bool is_mutable() const { return is_mutable_; }
};

// A block of expressions
class BlockExprAST : public ExprAST {
    std::vector<std::unique_ptr<ExprAST>> expressions_;

public:
    BlockExprAST(std::vector<std::unique_ptr<ExprAST>> expressions)
        : expressions_(std::move(expressions)) {}

    const std::vector<std::unique_ptr<ExprAST>>& get_expressions() const { return expressions_; }
};

// Expression class for if/then/else.
class IfExprAST : public ExprAST {
    std::unique_ptr<ExprAST> cond_, then_, else_;

public:
    IfExprAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<ExprAST> then,
              std::unique_ptr<ExprAST> else_)
        : cond_(std::move(cond)), then_(std::move(then)), else_(std::move(else_)) {}

    ExprAST* get_cond() const { return cond_.get(); }
    ExprAST* get_then() const { return then_.get(); }
    ExprAST* get_else() const { return else_.get(); }
};
