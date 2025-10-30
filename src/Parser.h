#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "AST.h"
#include "Lexer.h"
#include "Token.h"
#include <memory>
#include <vector>
#include <map>

namespace Chtholly {

// Operator precedence levels
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
    const std::vector<std::string>& errors() const { return m_errors; }

private:
    void nextToken();
    bool expectPeek(TokenType t);
    void peekError(TokenType t);
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<LetStatement> parseLetStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();

    std::unique_ptr<Expression> parseExpression(Precedence precedence);
    std::unique_ptr<Expression> parsePrefixExpression();
    std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);

    Precedence peekPrecedence();
    Precedence curPrecedence();

    Lexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;
    std::vector<std::string> m_errors;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
