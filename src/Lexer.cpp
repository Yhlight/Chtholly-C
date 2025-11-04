#include <Lexer.h>
#include <cctype>

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
    return makeToken(identifierType());
}

TokenType Lexer::identifierType() {
    switch (source[start]) {
        case 'a': return checkKeyword(1, 4, "rray", TokenType::ARRAY);
        case 'b': return checkKeyword(1, 3, "ool", TokenType::BOOL);
        case 'c':
            if (current - start > 1) {
                switch (source[start + 1]) {
                    case 'a': return checkKeyword(2, 2, "se", TokenType::CASE);
                    case 'h': return checkKeyword(2, 2, "ar", TokenType::CHAR);
                }
            }
            break;
        case 'd': return checkKeyword(1, 6, "ouble", TokenType::DOUBLE);
        case 'e':
            if (current - start > 1) {
                switch (source[start + 1]) {
                    case 'l': return checkKeyword(2, 2, "se", TokenType::ELSE);
                    case 'n': return checkKeyword(2, 2, "um", TokenType::ENUM);
                }
            }
            break;
        case 'f':
            if (current - start > 1) {
                switch (source[start + 1]) {
                    case 'o': return checkKeyword(2, 1, "r", TokenType::FOR);
                    case 'u': return checkKeyword(2, 2, "nc", TokenType::FUNC);
                }
            }
            break;
        case 'i':
            if (current - start > 1) {
                switch (source[start + 1]) {
                    case 'f': return TokenType::IF;
                    case 'm': return checkKeyword(2, 2, "pl", TokenType::IMPL);
                    case 'n': return checkKeyword(2, 1, "t", TokenType::INT);
                }
            }
            break;
        case 'l': return checkKeyword(1, 2, "et", TokenType::LET);
        case 'm': return checkKeyword(1, 2, "ut", TokenType::MUT);
        case 'o': return checkKeyword(1, 5, "ption", TokenType::OPTION);
        case 'r':
            if (current - start > 1) {
                switch (source[start + 1]) {
                    case 'e': return checkKeyword(2, 4, "turn", TokenType::RETURN);
                }
            }
            break;
        case 's':
            if (current - start > 1) {
                switch (source[start + 1]) {
                    case 't': return checkKeyword(2, 4, "ruct", TokenType::STRUCT);
                    case 'w': return checkKeyword(2, 4, "itch", TokenType::SWITCH);
                }
            }
            break;
        case 't': return checkKeyword(1, 4, "rait", TokenType::TRAIT);
        case 'v': return checkKeyword(1, 3, "oid", TokenType::VOID);
        case 'w': return checkKeyword(1, 4, "hile", TokenType::WHILE);
    }

    return TokenType::IDENTIFIER;
}

TokenType Lexer::checkKeyword(size_t start, size_t length, const char* rest, TokenType type) {
    if (current - this->start == start + length && source.substr(this->start + start, length) == rest) {
        return type;
    }
    return TokenType::IDENTIFIER;
}
