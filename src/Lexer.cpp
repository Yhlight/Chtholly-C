#include "Lexer.h"
#include <cctype>

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"struct", TokenType::STRUCT},
    {"func", TokenType::FUNC},
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"return", TokenType::RETURN},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"fallthrough", TokenType::FALLTHROUGH},
    {"break", TokenType::BREAK},
    {"public", TokenType::PUBLIC},
    {"private", TokenType::PRIVATE},
    {"self", TokenType::SELF},
    {"impl", TokenType::IMPL},
    {"trait", TokenType::TRAIT},
    {"import", TokenType::IMPORT},
    {"none", TokenType::NONE},
};

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

Token Lexer::nextToken() {
    skipWhitespace();
    start_ = current_;

    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE);
    }

    char c = advance();

    if (std::isalpha(c) || c == '_') {
        return identifier();
    }
    if (std::isdigit(c)) {
        return number();
    }

    switch (c) {
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case '[': return makeToken(TokenType::LEFT_BRACKET);
        case ']': return makeToken(TokenType::RIGHT_BRACKET);
        case ',': return makeToken(TokenType::COMMA);
        case '.': return makeToken(TokenType::DOT);
        case ';': return makeToken(TokenType::SEMICOLON);
        case ':': return makeToken(TokenType::COLON);
        case '?': return makeToken(TokenType::QUESTION);
        case '&':
            return makeToken(peek() == '&' ? (advance(), TokenType::LOGICAL_AND) : TokenType::AMPERSAND);
        case '|':
            return makeToken(peek() == '|' ? (advance(), TokenType::LOGICAL_OR) : TokenType::UNKNOWN); // Single '|' is not used yet.
        case '!':
            return makeToken(peek() == '=' ? (advance(), TokenType::BANG_EQUAL) : TokenType::BANG);
        case '=':
            if (peek() == '=') {
                advance();
                return makeToken(TokenType::EQUAL_EQUAL);
            } else if (peek() == '>') {
                advance();
                return makeToken(TokenType::FAT_ARROW);
            }
            return makeToken(TokenType::EQUAL);
        case '<':
            return makeToken(peek() == '=' ? (advance(), TokenType::LESS_EQUAL) : TokenType::LESS);
        case '>':
            return makeToken(peek() == '=' ? (advance(), TokenType::GREATER_EQUAL) : TokenType::GREATER);
        case '-':
            if (peek() == '>') {
                advance();
                return makeToken(TokenType::ARROW);
            } else if (peek() == '-') {
                advance();
                return makeToken(TokenType::MINUS_MINUS);
            } else if (peek() == '=') {
                advance();
                return makeToken(TokenType::MINUS_EQUAL);
            }
            return makeToken(TokenType::MINUS);
        case '+':
            if (peek() == '+') {
                advance();
                return makeToken(TokenType::PLUS_PLUS);
            } else if (peek() == '=') {
                advance();
                return makeToken(TokenType::PLUS_EQUAL);
            }
            return makeToken(TokenType::PLUS);
        case '*':
            return makeToken(peek() == '=' ? (advance(), TokenType::STAR_EQUAL) : TokenType::STAR);
        case '/':
            if (peek() == '/') {
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                return nextToken(); // Return the next token after the comment
            } else if (peek() == '*') {
                advance(); // Consume the '*'
                while (!isAtEnd()) {
                    if (peek() == '*' && source_[current_ + 1] == '/') {
                        break;
                    }
                    if (peek() == '\n') {
                        line_++;
                        col_ = 0;
                    }
                    advance();
                }
                if (isAtEnd()) {
                    return makeToken(TokenType::UNKNOWN); // Unterminated comment
                }
                advance(); // Consume the '*'
                advance(); // Consume the '/'
                return nextToken();
            } else if (peek() == '=') {
                advance();
                return makeToken(TokenType::SLASH_EQUAL);
            }
            return makeToken(TokenType::SLASH);
        case '%':
             return makeToken(peek() == '=' ? (advance(), TokenType::MODULO_EQUAL) : TokenType::MODULO);
        case '"':
            return stringLiteral();
        case '\'':
            return charLiteral();
    }

    return makeToken(TokenType::UNKNOWN);
}

char Lexer::advance() {
    if (!isAtEnd()) {
        current_++;
        col_++;
    }
    return source_[current_ - 1];
}

char Lexer::peek() const {
    if (isAtEnd()) {
        return '\0';
    }
    return source_[current_];
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line_++;
                col_ = 0;
                advance();
                break;
            default:
                return;
        }
    }
}

Token Lexer::makeToken(TokenType type) const {
    return {type, source_.substr(start_, current_ - start_), line_, col_ - (int)(current_ - start_)};
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return makeToken(it->second);
    }

    return makeToken(TokenType::IDENTIFIER);
}

Token Lexer::number() {
    bool isFloat = false;
    while (std::isdigit(peek())) {
        advance();
    }

    if (peek() == '.' && std::isdigit(source_[current_ + 1])) {
        isFloat = true;
        advance();
        while (std::isdigit(peek())) {
            advance();
        }
    }

    return makeToken(isFloat ? TokenType::NUMBER_FLOAT : TokenType::NUMBER_INT);
}

Token Lexer::stringLiteral() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line_++;
            col_ = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        return makeToken(TokenType::UNKNOWN); // Unterminated string
    }

    advance(); // The closing "
    return makeToken(TokenType::STRING);
}

Token Lexer::charLiteral() {
    if (peek() != '\'' && !isAtEnd()) {
        advance(); // The character itself
    } else {
        return makeToken(TokenType::UNKNOWN); // Empty or invalid char literal
    }

    if (peek() != '\'') {
         return makeToken(TokenType::UNKNOWN); // Unterminated char literal
    }
    advance(); // The closing '
    return makeToken(TokenType::CHAR);
}
