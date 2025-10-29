#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include <memory>
#include <map>

namespace Chtholly {

enum Precedence {
    LOWEST,
    EQUALS,      // ==
    LESSGREATER, // > or <
    SUM,         // +
    PRODUCT,     // *
    PREFIX,      // -X or !X
    CALL,        // myFunction(X)
};

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<Program> parseProgram();

private:
    using PrefixParseFn = std::unique_ptr<Expression> (Parser::*)();
    using InfixParseFn = std::unique_ptr<Expression> (Parser::*)(std::unique_ptr<Expression>);

    Lexer& lexer;
    Token currentToken;
    Token peekToken;
    std::map<TokenType, PrefixParseFn> prefixParseFns;
    std::map<TokenType, InfixParseFn> infixParseFns;
    std::map<TokenType, Precedence> precedences;

    void nextToken();
    bool expectPeek(TokenType t);
    Precedence peekPrecedence();
    Precedence curPrecedence();

    void registerPrefix(TokenType t, PrefixParseFn fn);
    void registerInfix(TokenType t, InfixParseFn fn);

    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<VariableDeclaration> parseVariableDeclaration();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<FunctionDeclaration> parseFunctionDeclaration();
    std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();
    std::unique_ptr<Expression> parseExpression(Precedence precedence);

    // Expression parsing functions
    std::unique_ptr<Expression> parseIdentifier();
    std::unique_ptr<Expression> parseIntegerLiteral();
    std::unique_ptr<Expression> parseIfExpression();
    std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);
    std::unique_ptr<Expression> parseFunctionCall(std::unique_ptr<Expression> function);
    std::vector<std::unique_ptr<Expression>> parseCallArguments();
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
