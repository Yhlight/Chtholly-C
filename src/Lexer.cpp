#include "Lexer.h"
#include <unordered_map>
#include <cctype>

namespace chtholly {

static std::unordered_map<std::string, TokenType> keywords;

static void initializeKeywords() {
    keywords["and"] = TokenType::AND;
    keywords["or"] = TokenType::OR;
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["true"] = TokenType::TRUE;
    keywords["false"] = TokenType::FALSE;
    keywords["func"] = TokenType::FUNC;
    keywords["for"] = TokenType::FOR;
    keywords["while"] = TokenType::WHILE;
    keywords["return"] = TokenType::RETURN;
    keywords["struct"] = TokenType::STRUCT;
    keywords["enum"] = TokenType::ENUM;
    keywords["let"] = TokenType::LET;
    keywords["mut"] = TokenType::MUT;
    keywords["const"] = TokenType::CONST;
    keywords["switch"] = TokenType::SWITCH;
    keywords["case"] = TokenType::CASE;
    keywords["default"] = TokenType::DEFAULT;
    keywords["break"] = TokenType::BREAK;
    keywords["fallthrough"] = TokenType::FALLTHROUGH;
    keywords["import"] = TokenType::IMPORT;
    keywords["public"] = TokenType::PUBLIC;
    keywords["private"] = TokenType::PRIVATE;
    keywords["self"] = TokenType::SELF;
    keywords["none"] = TokenType::NONE;
    keywords["trait"] = TokenType::TRAIT;
    keywords["impl"] = TokenType::IMPL;
    keywords["type_cast"] = TokenType::TYPE_CAST;
    keywords["int"] = TokenType::TYPE_INT;
    keywords["int8"] = TokenType::TYPE_INT8;
    keywords["int16"] = TokenType::TYPE_INT16;
    keywords["int32"] = TokenType::TYPE_INT32;
    keywords["int64"] = TokenType::TYPE_INT64;
    keywords["uint"] = TokenType::TYPE_UINT;
    keywords["uint8"] = TokenType::TYPE_UINT8;
    keywords["uint16"] = TokenType::TYPE_UINT16;
    keywords["uint32"] = TokenType::TYPE_UINT32;
    keywords["uint64"] = TokenType::TYPE_UINT64;
    keywords["float"] = TokenType::TYPE_FLOAT;
    keywords["double"] = TokenType::TYPE_DOUBLE;
    keywords["char"] = TokenType::TYPE_CHAR;
    keywords["string"] = TokenType::TYPE_STRING;
    keywords["bool"] = TokenType::TYPE_BOOL;
    keywords["void"] = TokenType::TYPE_VOID;
    keywords["array"] = TokenType::TYPE_ARRAY;
    keywords["option"] = TokenType::TYPE_OPTION;
    keywords["result"] = TokenType::TYPE_RESULT;
}


Lexer::Lexer(const std::string& source) : source(source) {
    if (keywords.empty()) {
        initializeKeywords();
    }
}

std::vector<Token> Lexer::scanTokens() {
    std::vector<Token> tokens;
    while (true) {
        Token token = scanToken();
        if (token.type != TokenType::ERROR || token.lexeme != "IGNORE") {
            tokens.push_back(token);
        }
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }
    return tokens;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}


Token Lexer::scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE);

    char c = advance();

    if (std::isalpha(c) || c == '_') return identifier();
    if (std::isdigit(c)) return number();

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
        case '%': return makeToken(match('=') ? TokenType::MODULO_EQUAL : TokenType::MODULO);
        case '&': return makeToken(TokenType::AMPERSAND);
        case '?': return makeToken(TokenType::QUESTION);

        case '-':
            if (match('-')) return makeToken(TokenType::MINUS_MINUS);
            if (match('=')) return makeToken(TokenType::MINUS_EQUAL);
            if (match('>')) return makeToken(TokenType::ARROW);
            return makeToken(TokenType::MINUS);
        case '+':
            if (match('+')) return makeToken(TokenType::PLUS_PLUS);
            if (match('=')) return makeToken(TokenType::PLUS_EQUAL);
            return makeToken(TokenType::PLUS);
        case '*':
            return makeToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR);
        case '!':
            return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=':
            return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<':
            return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case ':':
            return makeToken(match(':') ? TokenType::COLON_COLON : TokenType::COLON);

        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
                return {TokenType::ERROR, "IGNORE", {}, line};
            } else if (match('*')) {
                while (peek() != '*' || peekNext() != '/') {
                    if (isAtEnd()) return errorToken("Unterminated block comment.");
                    if (peek() == '\n') line++;
                    advance();
                }
                advance();
                advance();
                return {TokenType::ERROR, "IGNORE", {}, line};
            }
            else {
                return makeToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
            }

        case '"': return string();
    }

    return errorToken("Unexpected character.");
}


bool Lexer::isAtEnd() const {
    return current >= source.length();
}

char Lexer::advance() {
    return source[current++];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

Token Lexer::makeToken(TokenType type) const {
    return {type, source.substr(start, current - start), {}, line};
}

Token Lexer::makeToken(TokenType type, const Literal& literal) const {
     return {type, source.substr(start, current - start), literal, line};
}

Token Lexer::errorToken(const std::string& message) const {
    return {TokenType::ERROR, message, {}, line};
}

void Lexer::skipWhitespace() {
    for (;;) {
        if(isAtEnd()) return;
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
            default:
                return;
        }
    }
}

Token Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance();

    std::string value = source.substr(start + 1, current - start - 2);
    return makeToken(TokenType::STRING, value);
}

Token Lexer::number() {
    while (std::isdigit(peek())) advance();

    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek())) advance();
    }

    return makeToken(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();
    return makeToken(identifierType());
}

TokenType Lexer::identifierType() {
    auto it = keywords.find(source.substr(start, current - start));
    if (it == keywords.end()) return TokenType::IDENTIFIER;
    return it->second;
}

} // namespace chtholly
