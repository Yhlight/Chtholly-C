#include "Lexer.h"
#include <map>

namespace chtholly {

const std::map<std::string, TokenType> Lexer::keywords = {
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"func", TokenType::FUNC},
    {"struct", TokenType::STRUCT},
    {"enum", TokenType::ENUM},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"fallthrough", TokenType::FALLTHROUGH},
    {"break", TokenType::BREAK},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"none", TokenType::NONE},
    {"option", TokenType::OPTION},
    {"result", TokenType::RESULT},
    {"pass", TokenType::PASS},
    {"fail", TokenType::FAIL},
    {"is_pass", TokenType::IS_PASS},
    {"is_fail", TokenType::IS_FAIL},
    {"import", TokenType::IMPORT},
    {"type_cast", TokenType::TYPE_CAST},
    {"reflect", TokenType::REFLECT},
    {"meta", TokenType::META},
    {"util", TokenType::UTIL},
    {"self", TokenType::SELF},
    {"public", TokenType::PUBLIC},
    {"private", TokenType::PRIVATE},
    {"trait", TokenType::TRAIT},
    {"impl", TokenType::IMPL}
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (current < source.length()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", nullptr, line});
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': tokens.push_back({TokenType::LEFT_PAREN, "(", nullptr, line}); break;
        case ')': tokens.push_back({TokenType::RIGHT_PAREN, ")", nullptr, line}); break;
        case '{': tokens.push_back({TokenType::LEFT_BRACE, "{", nullptr, line}); break;
        case '}': tokens.push_back({TokenType::RIGHT_BRACE, "}", nullptr, line}); break;
        case '[': tokens.push_back({TokenType::LEFT_BRACKET, "[", nullptr, line}); break;
        case ']': tokens.push_back({TokenType::RIGHT_BRACKET, "]", nullptr, line}); break;
        case ',': tokens.push_back({TokenType::COMMA, ",", nullptr, line}); break;
        case '.': tokens.push_back({TokenType::DOT, ".", nullptr, line}); break;
        case '-':
            if (match('=')) {
                tokens.push_back({TokenType::MINUS_EQUAL, "-=", nullptr, line});
            } else if (match('>')) {
                tokens.push_back({TokenType::ARROW, "->", nullptr, line});
            } else {
                tokens.push_back({TokenType::MINUS, "-", nullptr, line});
            }
            break;
        case '+':
            if (match('=')) {
                tokens.push_back({TokenType::PLUS_EQUAL, "+=", nullptr, line});
            } else {
                tokens.push_back({TokenType::PLUS, "+", nullptr, line});
            }
            break;
        case ';': tokens.push_back({TokenType::SEMICOLON, ";", nullptr, line}); break;
        case '*':
            if (match('=')) {
                tokens.push_back({TokenType::STAR_EQUAL, "*=", nullptr, line});
            } else {
                tokens.push_back({TokenType::STAR, "*", nullptr, line});
            }
            break;
        case '?': tokens.push_back({TokenType::QUESTION, "?", nullptr, line}); break;
        case ':': tokens.push_back({TokenType::COLON, ":", nullptr, line}); break;
        case '!':
            tokens.push_back({match('=') ? TokenType::BANG_EQUAL : TokenType::BANG, c + std::string(match('=') ? "=" : ""), nullptr, line});
            break;
        case '=':
            tokens.push_back({match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL, c + std::string(match('=') ? "=" : ""), nullptr, line});
            break;
        case '<':
            tokens.push_back({match('=') ? TokenType::LESS_EQUAL : TokenType::LESS, c + std::string(match('=') ? "=" : ""), nullptr, line});
            break;
        case '>':
            tokens.push_back({match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER, c + std::string(match('=') ? "=" : ""), nullptr, line});
            break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && current < source.length()) advance();
            } else if (match('*')) {
                // A block comment goes until */ is found.
                while (peek() != '*' || peekNext() != '/') {
                    if (peek() == '\n') line++;
                    advance();
                }
                advance(); // consume the '*'
                advance(); // consume the '/'
            }
            else if (match('=')) {
                tokens.push_back({TokenType::SLASH_EQUAL, "/=", nullptr, line});
            }
            else {
                tokens.push_back({TokenType::SLASH, "/", nullptr, line});
            }
            break;
        case '&':
            if (match('&')) {
                tokens.push_back({TokenType::AMPERSAND_AMPERSAND, "&&", nullptr, line});
            } else {
                tokens.push_back({TokenType::AMPERSAND, "&", nullptr, line});
            }
            break;
        case '|':
            if (match('|')) {
                tokens.push_back({TokenType::PIPE_PIPE, "||", nullptr, line});
            } else {
                tokens.push_back({TokenType::PIPE, "|", nullptr, line});
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
                tokens.push_back({TokenType::ERROR, "Unexpected character.", nullptr, line});
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

bool Lexer::match(char expected) {
    if (current >= source.length()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Lexer::peek() {
    if (current >= source.length()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::string() {
    while (peek() != '"' && current < source.length()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (current >= source.length()) {
        tokens.push_back({TokenType::ERROR, "Unterminated string.", nullptr, line});
        return;
    }

    advance(); // The closing ".

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    tokens.push_back({TokenType::STRING, source.substr(start, current - start), value, line});
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();
    }

    double value = std::stod(source.substr(start, current - start));
    tokens.push_back({TokenType::NUMBER, source.substr(start, current - start), value, line});
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type;
    auto it = keywords.find(text);
    if (it == keywords.end()) {
        type = TokenType::IDENTIFIER;
    } else {
        type = it->second;
    }
    tokens.push_back({type, text, nullptr, line});
}

} // namespace chtholly
