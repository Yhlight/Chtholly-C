#include "Lexer.h"
#include <cctype>
#include <unordered_map>

namespace Chtholly {

static const std::unordered_map<std::string, TokenType> keywords = {
    {"func", TokenType::Func},
    {"let", TokenType::Let},
    {"mut", TokenType::Mut},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"return", TokenType::Return},
    {"struct", TokenType::Struct},
    {"import", TokenType::Import},
    {"public", TokenType::Public},
    {"private", TokenType::Private},
    {"fallthrough", TokenType::Fallthrough},
    {"trait", TokenType::Trait},
    {"impl", TokenType::Impl},
};

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

Token Lexer::nextToken() {
    skipWhitespace();

    char ch = currentChar();
    if (isalpha(ch) || ch == '_') {
        return identifier();
    }
    if (isdigit(ch)) {
        return number();
    }
    if (ch == '"') {
        return stringLiteral();
    }
    if (ch == '\'') {
        return charLiteral();
    }

    Token token;
    switch (ch) {
        case '=':
            if (source[position + 1] == '=') {
                token = {TokenType::Equal, "==", line, column};
                advance();
            } else {
                token = {TokenType::Assign, "=", line, column};
            }
            break;
        case '+':
            token = {TokenType::Plus, "+", line, column};
            break;
        case '-':
            token = {TokenType::Minus, "-", line, column};
            break;
        case '*':
            token = {TokenType::Asterisk, "*", line, column};
            break;
        case '/':
            token = {TokenType::Slash, "/", line, column};
            break;
        case '%':
            token = {TokenType::Percent, "%", line, column};
            break;
        case '!':
            if (source[position + 1] == '=') {
                token = {TokenType::NotEqual, "!=", line, column};
                advance();
            } else {
                token = {TokenType::Bang, "!", line, column};
            }
            break;
        case '<':
            if (source[position + 1] == '=') {
                token = {TokenType::LessEqual, "<=", line, column};
                advance();
            } else {
                token = {TokenType::Less, "<", line, column};
            }
            break;
        case '>':
            if (source[position + 1] == '=') {
                token = {TokenType::GreaterEqual, ">=", line, column};
                advance();
            } else {
                token = {TokenType::Greater, ">", line, column};
            }
            break;
        case '&':
            if (source[position + 1] == '&') {
                token = {TokenType::And, "&&", line, column};
                advance();
            }
            break;
        case '|':
            if (source[position + 1] == '|') {
                token = {TokenType::Or, "||", line, column};
                advance();
            }
            break;
        case '(':
            token = {TokenType::LParen, "(", line, column};
            break;
        case ')':
            token = {TokenType::RParen, ")", line, column};
            break;
        case '{':
            token = {TokenType::LBrace, "{", line, column};
            break;
        case '}':
            token = {TokenType::RBrace, "}", line, column};
            break;
        case '[':
            token = {TokenType::LBracket, "[", line, column};
            break;
        case ']':
            token = {TokenType::RBracket, "]", line, column};
            break;
        case ';':
            token = {TokenType::Semicolon, ";", line, column};
            break;
        case ':':
            token = {TokenType::Colon, ":", line, column};
            break;
        case ',':
            token = {TokenType::Comma, ",", line, column};
            break;
        case '.':
            token = {TokenType::Dot, ".", line, column};
            break;
        case '?':
            token = {TokenType::Question, "?", line, column};
            break;
        case '\0':
            token = {TokenType::Eof, "", line, column};
            break;
        default:
            token = {TokenType::Unknown, std::string(1, ch), line, column};
            break;
    }

    advance();
    return token;
}

char Lexer::currentChar() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

void Lexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    position++;
}

void Lexer::skipWhitespace() {
    while (currentChar() != '\0' && isspace(currentChar())) {
        advance();
    }
}

Token Lexer::identifier() {
    int start = position;
    while (isalnum(currentChar()) || currentChar() == '_') {
        advance();
    }
    std::string literal = source.substr(start, position - start);
    auto it = keywords.find(literal);
    if (it != keywords.end()) {
        return {it->second, literal, line, column};
    }
    return {TokenType::Identifier, literal, line, column};
}

Token Lexer::number() {
    int start = position;
    bool isDouble = false;
    while (isdigit(currentChar())) {
        advance();
    }
    if (currentChar() == '.') {
        isDouble = true;
        advance();
        while (isdigit(currentChar())) {
            advance();
        }
    }
    std::string literal = source.substr(start, position - start);
    return {isDouble ? TokenType::Double : TokenType::Integer, literal, line, column};
}

Token Lexer::stringLiteral() {
    advance(); // consume "
    int start = position;
    while (currentChar() != '"' && currentChar() != '\0') {
        advance();
    }
    std::string literal = source.substr(start, position - start);
    advance(); // consume "
    return {TokenType::String, literal, line, column};
}

Token Lexer::charLiteral() {
    advance(); // consume '
    char literal = currentChar();
    advance();
    if (currentChar() != '\'') {
        return {TokenType::Unknown, std::string(1, literal), line, column};
    }
    advance(); // consume '
    return {TokenType::Char, std::string(1, literal), line, column};
}

} // namespace Chtholly
