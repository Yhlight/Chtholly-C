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

private:
    void nextToken();
    bool expectPeek(TokenType t);
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<LetStatement> parseLetStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();

    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseFactor();

    Lexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
