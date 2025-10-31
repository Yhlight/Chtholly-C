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
    MEMBER,      // object.member
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
    std::unique_ptr<LetStatement> parseDeclarationStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<Statement> parseFunctionDeclaration();
    std::unique_ptr<StructStatement> parseStructStatement();
    std::unique_ptr<EnumStatement> parseEnumStatement();
    std::unique_ptr<ImportStatement> parseImportStatement();

    std::unique_ptr<Expression> parseExpression(Precedence precedence);
    std::unique_ptr<Expression> parsePrefixExpression();
    std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<Expression> parseIfExpression();
    std::unique_ptr<Expression> parseFunctionLiteral();
    std::unique_ptr<Expression> parseStructLiteral(std::unique_ptr<Expression> name);
    std::vector<std::unique_ptr<Identifier>> parseTemplateParams();
    std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();
    std::unique_ptr<Type> parseType();
    std::vector<std::unique_ptr<Type>> parseTemplateArgs();
    std::unique_ptr<Expression> parseCallExpression(std::unique_ptr<Expression> function);
    std::vector<std::unique_ptr<Expression>> parseCallArguments();

    Precedence peekPrecedence();
    Precedence curPrecedence();

    Lexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;
    std::vector<std::string> m_errors;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
