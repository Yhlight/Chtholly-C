#include "Parser.h"
#include <stdexcept>
#include <sstream>

namespace Chtholly {

static std::map<TokenType, Precedence> precedences = {
    {TokenType::Equal, EQUALS},
    {TokenType::NotEqual, EQUALS},
    {TokenType::LessThan, LESSGREATER},
    {TokenType::GreaterThan, LESSGREATER},
    {TokenType::Plus, SUM},
    {TokenType::Minus, SUM},
    {TokenType::Slash, PRODUCT},
    {TokenType::Asterisk, PRODUCT},
    {TokenType::LParen, CALL},
    {TokenType::Dot, MEMBER},
    {TokenType::DoubleColon, MEMBER},
};

Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    // Read two tokens, so m_curToken and m-peekToken are both set
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

bool Parser::expectPeek(TokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

void Parser::peekError(TokenType t) {
    std::stringstream ss;
    ss << "expected next token to be " << static_cast<int>(t) << ", got " << static_cast<int>(m_peekToken.type) << " instead";
    m_errors.push_back(ss.str());
}

Precedence Parser::peekPrecedence() {
    if (precedences.find(m_peekToken.type) != precedences.end()) {
        return precedences[m_peekToken.type];
    }
    return LOWEST;
}

Precedence Parser::curPrecedence() {
    if (precedences.find(m_curToken.type) != precedences.end()) {
        return precedences[m_curToken.type];
    }
    return LOWEST;
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();

    while (m_curToken.type != TokenType::Eof) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    switch (m_curToken.type) {
    case TokenType::Let:
    case TokenType::Mut:
        return parseDeclarationStatement();
    case TokenType::Return:
        return parseReturnStatement();
    case TokenType::Func:
        if (m_peekToken.type == TokenType::Identifier) {
            return parseFunctionDeclaration();
        }
        return parseExpressionStatement();
    case TokenType::Struct:
        return parseStructStatement();
    case TokenType::Enum:
        return parseEnumStatement();
    case TokenType::Trait:
        return parseTraitStatement();
    case TokenType::Impl:
        return parseImplStatement();
    case TokenType::Import:
        return parseImportStatement();
    default:
        return parseExpressionStatement();
    }
}

std::unique_ptr<LetStatement> Parser::parseDeclarationStatement() {
    auto stmt = std::make_unique<LetStatement>(m_curToken);
    stmt->isMutable = m_curToken.type == TokenType::Mut;

    if (!expectPeek(TokenType::Identifier)) {
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);

    if (m_peekToken.type == TokenType::Colon) {
        nextToken();
        nextToken();
        stmt->type = parseType();
    }

    if (!expectPeek(TokenType::Assign)) {
        return nullptr;
    }

    nextToken();

    stmt->value = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<EnumStatement> Parser::parseEnumStatement() {
    auto stmt = std::make_unique<EnumStatement>(m_curToken);

    if (!expectPeek(TokenType::Identifier)) {
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::LBrace)) {
        return nullptr;
    }

    nextToken();

    while (m_curToken.type != TokenType::RBrace && m_curToken.type != TokenType::Eof) {
        stmt->members.push_back(std::make_unique<Identifier>(m_curToken, m_curToken.literal));
        nextToken();
        if (m_curToken.type == TokenType::Comma) {
            nextToken();
        }
    }

    return stmt;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto stmt = std::make_unique<ReturnStatement>(m_curToken);

    nextToken();

    stmt->returnValue = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto stmt = std::make_unique<ExpressionStatement>(m_curToken);

    stmt->expression = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<StructStatement> Parser::parseStructStatement() {
    auto stmt = std::make_unique<StructStatement>(m_curToken);

    if (!expectPeek(TokenType::Identifier)) {
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);

    if (m_peekToken.type == TokenType::LessThan) {
        nextToken();
        stmt->templateParams = parseTemplateParams();
    }

    if (!expectPeek(TokenType::LBrace)) {
        return nullptr;
    }

    nextToken();

    while (m_curToken.type != TokenType::RBrace && m_curToken.type != TokenType::Eof) {
        bool isPublic = true;
        if (m_curToken.type == TokenType::Private) {
            isPublic = false;
            nextToken();
        } else if (m_curToken.type == TokenType::Public) {
            nextToken();
        }

        auto name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        nextToken();

        if (m_curToken.type == TokenType::Colon) { // Field
            nextToken();
            auto type = parseType();
            if (type) {
                auto field = std::make_unique<Field>(m_curToken, isPublic, std::move(name), std::move(type));
                stmt->members.push_back(std::move(field));
            }
        } else { // Method
            auto func = parseFunctionLiteral();
            if (func) {
                auto method = std::make_unique<Method>(m_curToken, isPublic, std::move(name), std::unique_ptr<FunctionLiteral>(static_cast<FunctionLiteral*>(func.release())));
                stmt->members.push_back(std::move(method));
            }
        }

        if (m_peekToken.type == TokenType::Comma) {
            nextToken();
        }
        nextToken();
    }

    return stmt;
}

std::unique_ptr<Expression> Parser::parseExpression(Precedence precedence) {
    std::unique_ptr<Expression> leftExp;
    switch (m_curToken.type) {
    case TokenType::Identifier:
        leftExp = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        break;
    case TokenType::Int:
        leftExp = std::make_unique<IntegerLiteral>(m_curToken, std::stoll(m_curToken.literal));
        break;
    case TokenType::String:
        leftExp = std::make_unique<StringLiteral>(m_curToken, m_curToken.literal);
        break;
    case TokenType::True:
    case TokenType::False:
        leftExp = std::make_unique<Boolean>(m_curToken, m_curToken.type == TokenType::True);
        break;
    case TokenType::Bang:
    case TokenType::Minus:
        leftExp = parsePrefixExpression();
        break;
    case TokenType::LParen:
        nextToken();
        leftExp = parseExpression(LOWEST);
        if (!expectPeek(TokenType::RParen)) {
            return nullptr;
        }
        break;
    case TokenType::If:
        leftExp = parseIfExpression();
        break;
    case TokenType::Func:
        leftExp = parseFunctionLiteral();
        break;
    default:
        return nullptr;
    }

    while (m_peekToken.type != TokenType::Semicolon && precedence < peekPrecedence()) {
        if (m_peekToken.type == TokenType::LParen) {
            nextToken();
            leftExp = parseCallExpression(std::move(leftExp));
        } else if (m_peekToken.type == TokenType::LessThan) {
            auto token = m_peekToken;
            nextToken();
            auto args = parseTemplateArgs();
            leftExp = std::make_unique<GenericInstantiation>(token, std::move(leftExp), std::move(args));

            if (m_peekToken.type == TokenType::LBrace) {
                nextToken();
                leftExp = parseStructLiteral(std::move(leftExp));
            }
        } else if (m_peekToken.type == TokenType::LBrace) {
            nextToken();
            leftExp = parseStructLiteral(std::move(leftExp));
        } else if (m_peekToken.type == TokenType::Dot) {
            nextToken();
            nextToken();
            auto member = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
            leftExp = std::make_unique<MemberAccessExpression>(m_curToken, std::move(leftExp), std::move(member));
        } else {
            nextToken();
            leftExp = parseInfixExpression(std::move(leftExp));
        }
    }

    return leftExp;
}

std::unique_ptr<Expression> Parser::parsePrefixExpression() {
    auto expression = std::make_unique<PrefixExpression>(m_curToken, m_curToken.literal);
    nextToken();
    expression->right = parseExpression(PREFIX);
    return expression;
}

std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left) {
    auto expression = std::make_unique<InfixExpression>(m_curToken, m_curToken.literal, std::move(left));
    auto precedence = curPrecedence();
    nextToken();
    expression->right = parseExpression(precedence);
    return expression;
}

std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    auto block = std::make_unique<BlockStatement>(m_curToken);

    nextToken();

    while (m_curToken.type != TokenType::RBrace && m_curToken.type != TokenType::Eof) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return block;
}

std::unique_ptr<Expression> Parser::parseIfExpression() {
    auto expression = std::make_unique<IfExpression>(m_curToken);

    if (!expectPeek(TokenType::LParen)) {
        return nullptr;
    }

    nextToken();
    expression->condition = parseExpression(LOWEST);

    if (!expectPeek(TokenType::RParen)) {
        return nullptr;
    }

    if (!expectPeek(TokenType::LBrace)) {
        return nullptr;
    }

    expression->consequence = parseBlockStatement();

    if (m_peekToken.type == TokenType::Else) {
        nextToken();

        if (!expectPeek(TokenType::LBrace)) {
            return nullptr;
        }

        expression->alternative = parseBlockStatement();
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parseStructLiteral(std::unique_ptr<Expression> name) {
    auto literal = std::make_unique<StructLiteral>(m_curToken, std::move(name));

    if (m_peekToken.type != TokenType::RBrace) {
        nextToken();

        auto key = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        if (!expectPeek(TokenType::Colon)) return nullptr;
        nextToken();
        auto value = parseExpression(LOWEST);
        literal->fields.emplace_back(std::move(key), std::move(value));

        while (m_peekToken.type == TokenType::Comma) {
            nextToken();
            nextToken();

            key = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
            if (!expectPeek(TokenType::Colon)) return nullptr;
            nextToken();
            value = parseExpression(LOWEST);
            literal->fields.emplace_back(std::move(key), std::move(value));
        }
    }

    if (!expectPeek(TokenType::RBrace)) return nullptr;

    return literal;
}

std::unique_ptr<Expression> Parser::parseFunctionLiteral() {
    auto literal = std::make_unique<FunctionLiteral>(m_curToken);

    if (m_peekToken.type == TokenType::LessThan) {
        nextToken();
        literal->templateParams = parseTemplateParams();
    }

    if (m_curToken.type != TokenType::LParen) {
        if (!expectPeek(TokenType::LParen)) {
            return nullptr;
        }
    }

    literal->parameters = parseFunctionParameters();

    if (m_peekToken.type == TokenType::Arrow) {
        nextToken();
        nextToken();
        literal->returnType = parseType();
    }

    if (!expectPeek(TokenType::LBrace)) {
        return nullptr;
    }

    literal->body = parseBlockStatement();

    return literal;
}

std::vector<std::unique_ptr<Identifier>> Parser::parseTemplateParams() {
    std::vector<std::unique_ptr<Identifier>> identifiers;

    if (m_curToken.type != TokenType::LessThan) {
        return identifiers;
    }

    nextToken();

    auto ident = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
    if (m_peekToken.type == TokenType::Question) {
        ident->constraints = parseConstraints();
    }
    identifiers.push_back(std::move(ident));

    while (m_peekToken.type == TokenType::Comma) {
        nextToken();
        nextToken();
        auto ident = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        if (m_peekToken.type == TokenType::Question) {
            ident->constraints = parseConstraints();
        }
        identifiers.push_back(std::move(ident));
    }

    if (!expectPeek(TokenType::GreaterThan)) {
        return {};
    }

    return identifiers;
}

std::unique_ptr<Statement> Parser::parseFunctionDeclaration() {
    auto let = std::make_unique<LetStatement>(m_curToken);
    let->isMutable = false;

    nextToken(); // Consume 'func'

    let->name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
    let->value = parseFunctionLiteral();

    return let;
}

std::unique_ptr<TraitStatement> Parser::parseTraitStatement() {
    auto stmt = std::make_unique<TraitStatement>(m_curToken);

    if (!expectPeek(TokenType::Identifier)) {
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::LBrace)) {
        return nullptr;
    }

    nextToken();

    while (m_curToken.type != TokenType::RBrace && m_curToken.type != TokenType::Eof) {
        bool isPublic = true;
        if (m_curToken.type == TokenType::Private) {
            isPublic = false;
            nextToken();
        } else if (m_curToken.type == TokenType::Public) {
            nextToken();
        }

        auto name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        nextToken();

        auto func = parseFunctionLiteral();
        if (func) {
            auto method = std::make_unique<Method>(m_curToken, isPublic, std::move(name), std::unique_ptr<FunctionLiteral>(static_cast<FunctionLiteral*>(func.release())));
            stmt->methods.push_back(std::move(method));
        }

        if (m_peekToken.type == TokenType::Comma) {
            nextToken();
        }
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ImplStatement> Parser::parseImplStatement() {
    auto stmt = std::make_unique<ImplStatement>(m_curToken);

    if (m_peekToken.type == TokenType::LessThan) {
        nextToken();
        stmt->templateParams = parseTemplateParams();
    }

    if (!expectPeek(TokenType::Identifier)) {
        return nullptr;
    }

    stmt->structName = std::make_unique<Identifier>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::Identifier)) {
        return nullptr;
    }

    stmt->traitName = std::make_unique<Identifier>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::LBrace)) {
        return nullptr;
    }

    nextToken();

    while (m_curToken.type != TokenType::RBrace && m_curToken.type != TokenType::Eof) {
        bool isPublic = true;
        if (m_curToken.type == TokenType::Private) {
            isPublic = false;
            nextToken();
        } else if (m_curToken.type == TokenType::Public) {
            nextToken();
        }

        auto name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        nextToken();

        auto func = parseFunctionLiteral();
        if (func) {
            auto method = std::make_unique<Method>(m_curToken, isPublic, std::move(name), std::unique_ptr<FunctionLiteral>(static_cast<FunctionLiteral*>(func.release())));
            stmt->methods.push_back(std::move(method));
        }

        if (m_peekToken.type == TokenType::Comma) {
            nextToken();
        }
        nextToken();
    }

    return stmt;
}

std::vector<std::unique_ptr<Identifier>> Parser::parseFunctionParameters() {
    std::vector<std::unique_ptr<Identifier>> identifiers;

    if (m_peekToken.type == TokenType::RParen) {
        nextToken();
        return identifiers;
    }

    nextToken();

    auto ident = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
    if (m_peekToken.type == TokenType::Colon) {
        nextToken();
        nextToken();
        ident->type = parseType();
    }
    identifiers.push_back(std::move(ident));

    while (m_peekToken.type == TokenType::Comma) {
        nextToken();
        nextToken();
        auto ident = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        if (m_peekToken.type == TokenType::Colon) {
            nextToken();
            nextToken();
            ident->type = parseType();
        }
        identifiers.push_back(std::move(ident));
    }

    if (!expectPeek(TokenType::RParen)) {
        return {};
    }

    return identifiers;
}

std::unique_ptr<Type> Parser::parseType() {
    auto typeName = std::make_unique<TypeName>(m_curToken, m_curToken.literal);
    if (m_peekToken.type == TokenType::LessThan) {
        nextToken();
        typeName->templateArgs = parseTemplateArgs();
    }
    return typeName;
}

std::unique_ptr<Expression> Parser::parseCallExpression(std::unique_ptr<Expression> function) {
    auto exp = std::make_unique<CallExpression>(m_curToken, std::move(function));
    if (m_curToken.type == TokenType::LessThan) {
        exp->templateArgs = parseTemplateArgs();
    }
    if (m_curToken.type != TokenType::LParen) {
        if (!expectPeek(TokenType::LParen)) {
            return nullptr;
        }
    }
    exp->arguments = parseCallArguments();
    return exp;
}

std::vector<std::unique_ptr<Type>> Parser::parseTemplateArgs() {
    std::vector<std::unique_ptr<Type>> types;

    if (m_curToken.type != TokenType::LessThan) {
        return types;
    }

    nextToken();

    types.push_back(parseType());

    while (m_peekToken.type == TokenType::Comma) {
        nextToken();
        nextToken();
        types.push_back(parseType());
    }

    if (!expectPeek(TokenType::GreaterThan)) {
        return {};
    }

    return types;
}

std::vector<std::unique_ptr<Expression>> Parser::parseCallArguments() {
    std::vector<std::unique_ptr<Expression>> args;

    if (m_curToken.type != TokenType::LParen) {
        return args;
    }

    if (m_peekToken.type == TokenType::RParen) {
        nextToken();
        return args;
    }

    nextToken();
    args.push_back(parseExpression(LOWEST));

    while (m_peekToken.type == TokenType::Comma) {
        nextToken();
        nextToken();
        args.push_back(parseExpression(LOWEST));
    }

    if (!expectPeek(TokenType::RParen)) {
        return {};
    }

    return args;
}

std::vector<std::unique_ptr<Constraint>> Parser::parseConstraints() {
    std::vector<std::unique_ptr<Constraint>> constraints;

    while (m_peekToken.type == TokenType::Question) {
        nextToken(); // consume '?'
        nextToken(); // consume trait name
        auto constraint = std::make_unique<Constraint>(m_curToken, std::make_unique<Identifier>(m_curToken, m_curToken.literal));
        constraints.push_back(std::move(constraint));
    }

    return constraints;
}

std::unique_ptr<ImportStatement> Parser::parseImportStatement() {
    auto stmt = std::make_unique<ImportStatement>(m_curToken);

    nextToken();

    stmt->path = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

} // namespace Chtholly
