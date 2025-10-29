#include "Lexer.h"
#include "Token.h"
#include <unordered_map>

namespace Chtholly
{

static const std::unordered_map<std::string, TokenType> keywords = {
    {"func", TokenType::Func},
    {"let", TokenType::Let},
    {"mut", TokenType::Mut},
    {"return", TokenType::Return},
    {"struct", TokenType::Struct},
    {"public", TokenType::Public},
    {"private", TokenType::Private},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"fallthrough", TokenType::Fallthrough},
    {"for", TokenType::For},
    {"while", TokenType::While},
    {"break", TokenType::Break},
    {"import", TokenType::Import},
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::EndOfFile);
    return tokens;
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

Token Lexer::makeToken(TokenType type) const {
    return {type, source.substr(start, current - start), std::monostate{}, line, (int)start};
}

Token Lexer::nextToken()
{
    skipWhitespace();
    start = current;

    if (isAtEnd())
    {
        return makeToken(TokenType::EndOfFile);
    }

    char c = advance();

    if (isalpha(c) || c == '_') return identifier();
    if (isdigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TokenType::LParen);
        case ')': return makeToken(TokenType::RParen);
        case '{': return makeToken(TokenType::LBrace);
        case '}': return makeToken(TokenType::RBrace);
        case '[': return makeToken(TokenType::LBracket);
        case ']': return makeToken(TokenType::RBracket);
        case ';': return makeToken(TokenType::Semicolon);
        case ':': return makeToken(TokenType::Colon);
        case ',': return makeToken(TokenType::Comma);
        case '.': return makeToken(TokenType::Unknown); // Dot not implemented yet
        case '+': return makeToken(TokenType::Plus);
        case '-': return makeToken(match('>') ? TokenType::Arrow : TokenType::Minus);
        case '*': return makeToken(TokenType::Star);
        case '/': return makeToken(TokenType::Slash);
        case '%': return makeToken(TokenType::Mod);
        case '!': return makeToken(match('=') ? TokenType::NotEqual : TokenType::LogicalNot);
        case '=': return makeToken(match('=') ? TokenType::Equal : TokenType::Assign);
        case '<': return makeToken(match('=') ? TokenType::LessEqual : TokenType::Less);
        case '>': return makeToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
        case '&': return makeToken(match('&') ? TokenType::LogicalAnd : TokenType::Ampersand);
        case '|': return makeToken(match('|') ? TokenType::LogicalOr : TokenType::BitwiseOr);
        case '"': return stringLiteral();
        case '\'': return charLiteral();
    }


    return {TokenType::Unknown, std::string(1, c), std::monostate{}, line, (int)start};
}

char Lexer::advance()
{
    return source[current++];
}

char Lexer::peek() const
{
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const
{
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() const
{
    return current >= source.length();
}

void Lexer::skipWhitespace()
{
    while (true)
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            line++;
            advance();
            break;
        case '/':
            if (peekNext() == '/')
            {
                while (peek() != '\n' && !isAtEnd())
                {
                    advance();
                }
            }
            else
            {
                return;
            }
            break;
        default:
            return;
        }
    }
}

Token Lexer::identifier()
{
    while (isalnum(peek()) || peek() == '_')
    {
        advance();
    }

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::Identifier;

    if (keywords.count(text))
    {
        type = keywords.at(text);
    } else if (text == "true" || text == "false") {
        type = TokenType::Boolean;
        return {type, text, text == "true", line, (int)start};
    }

    return {type, text, text, line, (int)start};
}


Token Lexer::number()
{
    while (isdigit(peek()))
    {
        advance();
    }

    if (peek() == '.' && isdigit(peekNext()))
    {
        advance();
        while (isdigit(peek()))
        {
            advance();
        }
        std::string text = source.substr(start, current - start);
        return {TokenType::Double, text, std::stod(text), line, (int)start};
    }

    std::string text = source.substr(start, current - start);
    return {TokenType::Integer, text, std::stoll(text), line, (int)start};
}

Token Lexer::stringLiteral()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd())
    {
        return {TokenType::Unknown, "Unterminated string.", std::monostate{}, line, (int)start};
    }

    advance(); // The closing ".

    std::string text = source.substr(start, current - start);
    std::string value = source.substr(start + 1, current - start - 2);
    return {TokenType::String, text, value, line, (int)start};
}

Token Lexer::charLiteral()
{
    char value = advance();
    if (peek() != '\'')
    {
        return {TokenType::Unknown, "Unterminated char.", std::monostate{}, line, (int)start};
    }
    advance(); // The closing '.

    std::string text = source.substr(start, current - start);
    return {TokenType::Char, text, value, line, (int)start};
}

} // namespace Chtholly
