#include "../include/Lexer.h"
#include <iostream>

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"struct", TokenType::STRUCT},
    {"func", TokenType::FUNC},
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"break", TokenType::BREAK},
    {"fallthrough", TokenType::FALLTHROUGH},
    {"return", TokenType::RETURN},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"none", TokenType::NONE},
    {"enum", TokenType::ENUM},
    {"import", TokenType::IMPORT},
    {"public", TokenType::PUBLIC},
    {"private", TokenType::PRIVATE},
    {"self", TokenType::SELF},
    {"trait", TokenType::TRAIT},
    {"impl", TokenType::IMPL},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR}
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
        case ';': addToken(TokenType::SEMICOLON); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '?': addToken(TokenType::QUESTION); break;
        case ':':
            addToken(match(':') ? TokenType::COLON_COLON : TokenType::COLON);
            break;
        case '-':
            if (match('>')) {
                addToken(TokenType::ARROW);
            } else if (match('-')) {
                addToken(TokenType::MINUS_MINUS);
            } else if (match('=')) {
                addToken(TokenType::MINUS_EQUAL);
            } else {
                addToken(TokenType::MINUS);
            }
            break;
        case '+':
            if (match('+')) {
                addToken(TokenType::PLUS_PLUS);
            } else if (match('=')) {
                addToken(TokenType::PLUS_EQUAL);
            } else {
                addToken(TokenType::PLUS);
            }
            break;
        case '*':
            addToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR);
            break;
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
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }

                if (isAtEnd()) {
                    std::cerr << "Line " << line << ": Unterminated multi-line comment." << std::endl;
                } else {
                    advance(); // consume the '*'
                    advance(); // consume the '/'
                }
            }
            else {
                addToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
            }
            break;
        case '&':
            if (match('&')) {
                addToken(TokenType::AND);
            }
            break;
        case '|':
            if (match('|')) {
                addToken(TokenType::OR);
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
        case '\'': {
            if (isAtEnd()) {
                std::cerr << "Line " << line << ": Unterminated character literal at EOF." << std::endl;
                break;
            }

            if (peek() == '\n' || isAtEnd()) {
                std::cerr << "Line " << line << ": Unterminated character literal." << std::endl;
                break;
            }

            char value = advance(); // get the character

            if (peek() == '\'') {
                advance(); // consume the closing quote
                addToken(TokenType::CHAR, std::string(1, value));
            } else {
                std::cerr << "Line " << line << ": Multi-character or unterminated character literal." << std::endl;
                // Recovery: assume the literal was just one char long and the next char is a different token.
            }
            break;
        }
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
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    std::string text = source.substr(start, current - start);
    if (literal.empty()) {
        tokens.emplace_back(type, text, line);
    } else {
        tokens.emplace_back(type, literal, line);
    }
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

    advance(); // The closing ".

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::number() {
    bool isDouble = false;
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        isDouble = true;
        // Consume the ".'
        advance();

        while (isdigit(peek())) advance();
    }

    addToken(isDouble ? TokenType::DOUBLE : TokenType::INTEGER,
             source.substr(start, current - start));
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
