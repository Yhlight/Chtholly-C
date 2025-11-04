#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "Lexer.h"
#include "Token.h"
#include "TokenType.h"

const std::map<std::string, TokenType> Lexer::keywords = {
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE},
    {"func", TokenType::FUNC},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"struct", TokenType::STRUCT},
    {"enum", TokenType::ENUM},
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"public", TokenType::PUBLIC},
    {"private", TokenType::PRIVATE},
    {"self", TokenType::SELF},
    {"import", TokenType::IMPORT},
    {"trait", TokenType::TRAIT},
    {"impl", TokenType::IMPL},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"break", TokenType::BREAK},
    {"fallthrough", TokenType::FALLTHROUGH},
    {"type_cast", TokenType::TYPE_CAST},
    {"int", TokenType::INT},
    {"int8", TokenType::INT8},
    {"int16", TokenType::INT16},
    {"int32", TokenType::INT32},
    {"int64", TokenType::INT64},
    {"uint", TokenType::UINT},
    {"uint8", TokenType::UINT8},
    {"uint16", TokenType::UINT16},
    {"uint32", TokenType::UINT32},
    {"uint64", TokenType::UINT64},
    {"float", TokenType::FLOAT},
    {"double", TokenType::DOUBLE},
    {"long", TokenType::LONG_DOUBLE},
    {"bool", TokenType::BOOL},
    {"void", TokenType::VOID},
    {"string", TokenType::STRING_TYPE},
    {"array", TokenType::ARRAY},
    {"function", TokenType::FUNCTION},
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", line);
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
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
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case '?': addToken(TokenType::QUESTION); break;

        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '-':
            addToken(match('>') ? TokenType::ARROW : TokenType::MINUS);
            break;
        case ':':
            addToken(match(':') ? TokenType::COLON_COLON : TokenType::COLON);
            break;

        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                // A multi-line comment
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // consume the '*'
                    advance(); // consume the '/'
                }
            }
            else {
                addToken(TokenType::SLASH);
            }
            break;

        case '&':
            if (peek() == 'm' && peekNext() == 'u' && source[current + 2] == 't') {
                advance(); // m
                advance(); // u
                advance(); // t
                addToken(TokenType::AMPERSAND_MUT);
            } else {
                addToken(TokenType::AMPERSAND);
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
        case '\'': // char literal
            while (peek() != '\'' && !isAtEnd()) advance();
            if (isAtEnd()) {
                std::cerr << "Line " << line << ": Unterminated char literal." << std::endl;
                break;
            }
            advance(); // consume the closing '
            addToken(TokenType::CHAR);
            break;

        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                std::cerr << "Line " << line << ": Unexpected character '" << c << "'" << std::endl;
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, line);
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

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Line " << line << ": Unterminated string." << std::endl;
        return;
    }

    advance(); // The closing ".

    addToken(TokenType::STRING);
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
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(text)) {
        type = keywords.at(text);
    }
    addToken(type);
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}
