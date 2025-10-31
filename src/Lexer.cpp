#include "Lexer.h"
#include <cctype>
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"let", TokenType::Let},
    {"mut", TokenType::Mut},
    {"func", TokenType::Func},
    {"struct", TokenType::Struct},
    {"enum", TokenType::Enum},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"switch", TokenType::Switch},
    {"case", TokenType::SwitchCase},
    {"return", TokenType::Return},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"none", TokenType::None},
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

Token Lexer::next_token() {
    skip_whitespace();

    if (position >= source.length()) {
        return {TokenType::Eof, ""};
    }

    char current_char = advance();

    if (isalpha(current_char)) {
        position--;
        return identifier();
    }

    if (isdigit(current_char)) {
        position--;
        return number();
    }

    if (current_char == '"') {
        return string_literal();
    }

    if (current_char == '\'') {
        return char_literal();
    }

    switch (current_char) {
        case '+':
            return {TokenType::Plus, "+"};
        case '-':
            return {TokenType::Minus, "-"};
        case '*':
            return {TokenType::Asterisk, "*"};
        case '/':
            if (peek() == '/' || peek() == '*') {
                skip_comment();
                return next_token();
            }
            return {TokenType::Slash, "/"};
        case '%':
            return {TokenType::Percent, "%"};
        case '=':
            if (peek() == '=') {
                advance();
                return {TokenType::DoubleEqual, "=="};
            }
            return {TokenType::Equal, "="};
        case '!':
            if (peek() == '=') {
                advance();
                return {TokenType::NotEqual, "!="};
            }
            return {TokenType::LogicalNot, "!"};
        case '<':
            if (peek() == '=') {
                advance();
                return {TokenType::LessThanOrEqual, "<="};
            }
            if (peek() == '<') {
                advance();
                return {TokenType::LeftShift, "<<"};
            }
            return {TokenType::LessThan, "<"};
        case '>':
            if (peek() == '=') {
                advance();
                return {TokenType::GreaterThanOrEqual, ">="};
            }
            if (peek() == '>') {
                advance();
                return {TokenType::RightShift, ">>"};
            }
            return {TokenType::GreaterThan, ">"};
        case '&':
            if (peek() == '&') {
                advance();
                return {TokenType::LogicalAnd, "&&"};
            }
            return {TokenType::BitwiseAnd, "&"};
        case '|':
            if (peek() == '|') {
                advance();
                return {TokenType::LogicalOr, "||"};
            }
            return {TokenType::BitwiseOr, "|"};
        case '^':
            return {TokenType::BitwiseXor, "^"};
        case '~':
            return {TokenType::BitwiseNot, "~"};
        case '(':
            return {TokenType::LParen, "("};
        case ')':
            return {TokenType::RParen, ")"};
        case '{':
            return {TokenType::LBrace, "{"};
        case '}':
            return {TokenType::RBrace, "}"};
        case '[':
            return {TokenType::LBracket, "["};
        case ']':
            return {TokenType::RBracket, "]"};
        case ',':
            return {TokenType::Comma, ","};
        case ':':
            return {TokenType::Colon, ":"};
        case ';':
            return {TokenType::Semicolon, ";"};
        case '.':
            return {TokenType::Period, "."};
        default:
            return {TokenType::Illegal, std::string(1, current_char)};
    }
}

void Lexer::skip_whitespace() {
    while (position < source.length() && isspace(source[position])) {
        position++;
    }
}

void Lexer::skip_comment() {
    if (source[position] == '/') { // Single-line comment
        while (position < source.length() && source[position] != '\n') {
            position++;
        }
    } else if (source[position] == '*') { // Multi-line comment
        position++;
        while (position + 1 < source.length() && (source[position] != '*' || source[position + 1] != '/')) {
            position++;
        }
        if (position + 1 < source.length()) {
            position += 2;
        } else {
            // Unterminated multi-line comment, handle error as needed
            position = source.length();
        }
    }
}

Token Lexer::identifier() {
    size_t start = position;
    while (position < source.length() && isalnum(source[position])) {
        advance();
    }
    std::string literal = source.substr(start, position - start);
    if (keywords.count(literal)) {
        return {keywords.at(literal), literal};
    }
    return {TokenType::Identifier, literal};
}

Token Lexer::number() {
    size_t start = position;
    bool is_float = false;
    while (position < source.length() && isdigit(source[position])) {
        advance();
    }
    if (peek() == '.' && isdigit(source[position + 1])) {
        is_float = true;
        advance();
        while (position < source.length() && isdigit(source[position])) {
            advance();
        }
    }
    std::string literal = source.substr(start, position - start);
    return {is_float ? TokenType::Float : TokenType::Integer, literal};
}

Token Lexer::string_literal() {
    size_t start = position;
    while (position < source.length() && source[position] != '"') {
        advance();
    }
    std::string literal = source.substr(start, position - start);
    advance(); // Consume the closing quote
    return {TokenType::StringLiteral, literal};
}

Token Lexer::char_literal() {
    char literal = advance();
    if (peek() == '\'') {
        advance();
    }
    return {TokenType::CharLiteral, std::string(1, literal)};
}

char Lexer::peek() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char Lexer::advance() {
    return source[position++];
}
