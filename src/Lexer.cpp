#include "Lexer.h"
#include <iostream>
#include <map>
#include <stdexcept>

static std::map<std::string, TokenType> keywords = {
    {"func", TokenType::FUNC},
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"fallthrough", TokenType::FALLTHROUGH},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},
    {"struct", TokenType::STRUCT},
    {"enum", TokenType::ENUM},
    {"public", TokenType::PUBLIC},
    {"private", TokenType::PRIVATE},
    {"import", TokenType::IMPORT},
    {"trait", TokenType::TRAIT},
    {"impl", TokenType::IMPL},
    {"option", TokenType::OPTION},
    {"none", TokenType::NONE},
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, source.substr(start, current - start));
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    tokens.push_back({type, literal, line});
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string at line " + std::to_string(line));
    }

    advance(); // The closing ".

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();
    }

    addToken(TokenType::NUMBER);
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords[text] : TokenType::IDENTIFIER;
    addToken(type);
}

void Lexer::singleLineComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

void Lexer::multiLineComment() {
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        throw std::runtime_error("Unterminated multi-line comment at line " + std::to_string(line));
    }

    // Consume the "*/"
    advance();
    advance();
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
        case ';': addToken(TokenType::SEMICOLON); break;
        case ':': addToken(TokenType::COLON); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case '*': addToken(TokenType::STAR); break;
        case '%': addToken(TokenType::MODULO); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '&': addToken(match('&') ? TokenType::AMPERSAND_AMPERSAND : TokenType::AMPERSAND); break;
        case '|': addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE); break;
        case '/':
            if (match('/')) {
                singleLineComment();
            } else if (match('*')) {
                multiLineComment();
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            line++;
            break;

        case '"': string(); break;

        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                throw std::runtime_error("Unexpected character '" + std::string(1, c) + "' at line " + std::to_string(line));
            }
            break;
    }
}
