#include "Lexer.h"
#include "TokenType.h"
#include <map>
#include <iostream>

namespace {
    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
                c == '_';
    }

    bool isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }
}

static std::map<std::string, TokenType> keywords = {
    {"and",    TokenType::AND},
    {"struct", TokenType::STRUCT},
    {"else",   TokenType::ELSE},
    {"false",  TokenType::FALSE},
    {"for",    TokenType::FOR},
    {"func",   TokenType::FUNC},
    {"if",     TokenType::IF},
    {"nil",    TokenType::NIL},
    {"or",     TokenType::OR},
    {"print",  TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super",  TokenType::SUPER},
    {"this",   TokenType::THIS},
    {"true",   TokenType::TRUE},
    {"let",    TokenType::LET},
    {"while",  TokenType::WHILE},
    {"mut",    TokenType::MUT},
    {"switch", TokenType::SWITCH},
    {"case",   TokenType::CASE},
    {"break",  TokenType::BREAK},
    {"fallthrough", TokenType::FALLTHROUGH},
    {"enum",   TokenType::ENUM},
    {"trait",  TokenType::TRAIT},
    {"impl",   TokenType::IMPL},
    {"import", TokenType::IMPORT}
};


Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::EOF_TOKEN, "", nullptr, line);
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(match('>') ? TokenType::MINUS_GREATER : TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case ':': addToken(TokenType::COLON); break;
        case '&': addToken(TokenType::AMPERSAND); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                while (peek() != '*' && peekNext() != '/' && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) advance();
                if (!isAtEnd()) advance();
            }
            else {
                addToken(TokenType::SLASH);
            }
            break;

        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;

        case '"': string(); break;

        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            }
            else {
                std::cerr << "Line " << line << ": Unexpected character." << std::endl;
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, nullptr);
}

void Lexer::addToken(TokenType type, const std::any& literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Line " << line << ": Unterminated string." << std::endl;
        return;
    }

    advance();

    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
        addToken(TokenType::DOUBLE, std::stod(source.substr(start, current - start)));
        return;
    }

    addToken(TokenType::INTEGER, std::stoi(source.substr(start, current - start)));
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type;
    if (keywords.count(text)) {
        type = keywords[text];
    } else {
        type = TokenType::IDENTIFIER;
    }

    addToken(type);
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}
