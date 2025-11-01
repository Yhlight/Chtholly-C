#include "Lexer.h"
#include <unordered_map>
#include <cctype>
#include <vector>

namespace chtholly {

// Global map for keywords
static const std::unordered_map<std::string, TokenType> keywords = {
    {"if",     TokenType::IF},
    {"else",   TokenType::ELSE},
    {"true",   TokenType::TRUE},
    {"false",  TokenType::FALSE},
    {"for",    TokenType::FOR},
    {"func",   TokenType::FUNC},
    {"let",    TokenType::LET},
    {"mut",    TokenType::MUT},
    {"return", TokenType::RETURN},
    {"struct", TokenType::STRUCT},
    {"enum",   TokenType::ENUM},
    {"import", TokenType::IMPORT},
    {"switch", TokenType::SWITCH},
    {"case",   TokenType::CASE},
    {"default",TokenType::DEFAULT},
    {"break",  TokenType::BREAK},
    {"fallthrough", TokenType::FALLTHROUGH},
    {"public", TokenType::PUBLIC},
    {"private",TokenType::PRIVATE},
    {"trait",  TokenType::TRAIT},
    {"impl",   TokenType::IMPL},
};


Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    std::vector<Token> tokens;
    while (peek() != '\0') {
        tokens.push_back(scanToken());
    }
    tokens.push_back(Token(TokenType::END_OF_FILE, "", line));
    return tokens;
}


Token Lexer::makeToken(TokenType type) const {
    return Token(type, source.substr(start, current - start), line);
}

Token Lexer::errorToken(const std::string& message) const {
    return Token(TokenType::ERROR, message, line);
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

void Lexer::skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
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
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

TokenType Lexer::identifierType() {
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}


Token Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();
    return makeToken(identifierType());
}

Token Lexer::number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }

    return makeToken(TokenType::NUMBER);
}

Token Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance(); // The closing ".
    return makeToken(TokenType::STRING);
}

Token Lexer::scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE);
    }

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case '[': return makeToken(TokenType::LEFT_BRACKET);
        case ']': return makeToken(TokenType::RIGHT_BRACKET);
        case ';': return makeToken(TokenType::SEMICOLON);
        case ',': return makeToken(TokenType::COMMA);
        case '.': return makeToken(TokenType::DOT);
        case '-': return makeToken(match('=') ? TokenType::MINUS_EQUAL : (match('>') ? TokenType::ARROW : TokenType::MINUS));
        case '+': return makeToken(match('=') ? TokenType::PLUS_EQUAL : TokenType::PLUS);
        case '/': return makeToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
        case '*': return makeToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR);
        case '%': return makeToken(match('=') ? TokenType::PERCENT_EQUAL : TokenType::PERCENT);
        case '!': return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=': return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<': return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>': return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case ':': return makeToken(match(':') ? TokenType::COLON_COLON : TokenType::COLON);
        case '&': return makeToken(match('&') ? TokenType::AND : TokenType::AMPERSAND);
        case '|': return makeToken(match('|') ? TokenType::OR : TokenType::PIPE);
        case '"': return string();
    }

    return errorToken("Unexpected character.");
}

char Lexer::advance() {
    return source[current++];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

} // namespace chtholly
