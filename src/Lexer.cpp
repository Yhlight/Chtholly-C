#include <Lexer.h>
#include <cctype>
#include <map>

static const std::map<std::string, TokenType> keywords = {
    {"func", TokenType::FUNC},
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"default", TokenType::DEFAULT},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},
    {"struct", TokenType::STRUCT},
    {"enum", TokenType::ENUM},
    {"trait", TokenType::TRAIT},
    {"impl", TokenType::IMPL},
    {"import", TokenType::IMPORT},
    {"int", TokenType::INT},
    {"double", TokenType::DOUBLE},
    {"char", TokenType::CHAR},
    {"bool", TokenType::BOOL},
    {"string", TokenType::STRING},
    {"void", TokenType::VOID},
    {"array", TokenType::ARRAY},
    {"option", TokenType::OPTION},
    {"result", TokenType::RESULT},
    {"function", TokenType::FUNCTION},
};

Lexer::Lexer(const std::string& source) : source(source) {}

Token Lexer::nextToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE);
    }

    char c = advance();

    if (std::isalpha(c)) return identifier();
    if (std::isdigit(c)) return number();

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
        case '?': return makeToken(TokenType::QUESTION);
        case '+': return makeToken(TokenType::PLUS);
        case '/': return makeToken(TokenType::SLASH);
        case '*': return makeToken(TokenType::STAR);
        case '%': return makeToken(TokenType::PERCENT);
        case '&': return makeToken(match('&') ? TokenType::AND_AND : TokenType::AND);
        case '|': return makeToken(match('|') ? TokenType::OR_OR : TokenType::OR);
        case '=': return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '!': return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '<': return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>': return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '-': return makeToken(match('>') ? TokenType::ARROW : TokenType::MINUS);
        case ':': return makeToken(match(':') ? TokenType::COLON_COLON : TokenType::COLON);
        case '"': return string();
    }

    return errorToken("Unexpected character.");
}

char Lexer::advance() {
    current++;
    column++;
    return source[current - 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

Token Lexer::makeToken(TokenType type) {
    return Token{type, source.substr(start, current - start), line, column};
}

Token Lexer::errorToken(const std::string& message) {
    return Token{TokenType::UNKNOWN, message, line, column};
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
                line++;
                column = 1;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    advance();
                    advance();
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!isAtEnd()) advance();
                    if (!isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
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
    column++;
    return true;
}

Token Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance();
    return makeToken(TokenType::STRING_LITERAL);
}

Token Lexer::number() {
    while (std::isdigit(peek())) advance();

    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek())) advance();
        return makeToken(TokenType::FLOATING_LITERAL);
    }

    return makeToken(TokenType::INTEGER_LITERAL);
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return makeToken(it->second);
    }
    return makeToken(TokenType::IDENTIFIER);
}
