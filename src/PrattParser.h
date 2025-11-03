#pragma once

#include "Token.h"
#include "AST/Expr.h"
#include <vector>
#include <map>
#include <functional>

class Parser;

enum class Precedence {
    NONE,
    ASSIGNMENT,  // =
    OR,          // or
    AND,         // and
    EQUALITY,    // == !=
    COMPARISON,  // < > <= >=
    TERM,        // + -
    FACTOR,      // * /
    UNARY,       // ! -
    CALL,        // . ()
    PRIMARY
};

class PrattParser {
public:
    PrattParser(Parser& parser);
    std::unique_ptr<Expr> parse();

private:
    using PrefixParseFn = std::function<std::unique_ptr<Expr>()>;
    using InfixParseFn = std::function<std::unique_ptr<Expr>(std::unique_ptr<Expr>)>;

    struct ParseRule {
        PrefixParseFn prefix;
        InfixParseFn infix;
        Precedence precedence;
    };

    Parser& parser;
    std::map<TokenType, ParseRule> rules;

    std::unique_ptr<Expr> parsePrecedence(Precedence precedence);
    const ParseRule& getRule(TokenType type);

    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> binary(std::unique_ptr<Expr> left);
    std::unique_ptr<Expr> grouping();
    std::unique_ptr<Expr> literal();
    std::unique_ptr<Expr> variable();
    std::unique_ptr<Expr> call(std::unique_ptr<Expr> left);
    std::unique_ptr<Expr> genericCall(std::unique_ptr<Expr> callee);
    std::unique_ptr<Expr> get(std::unique_ptr<Expr> left);
    std::unique_ptr<Expr> assignment(std::unique_ptr<Expr> left);
    std::unique_ptr<Expr> borrow();
    std::unique_ptr<Expr> lambda();
    std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee, std::vector<TypeInfo> generic_args);
    bool LA_is_generic_call();
};
