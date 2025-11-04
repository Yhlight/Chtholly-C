#include "Lexer.h"
#include <map>
#include <iostream>

namespace chtholly {

static std::map<std::string, TokenType> keywords;

static void initializeKeywords() {
    keywords["let"] = TokenType::LET;
    keywords["mut"] = TokenType::MUT;
    keywords["func"] = TokenType::FUNC;
    keywords["return"] = TokenType::RETURN;
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["while"] = TokenType::WHILE;
    keywords["for"] = TokenType::FOR;
    keywords["switch"] = TokenType::SWITCH;
    keywords["case"] = TokenType::CASE;
    keywords["break"] = TokenType::BREAK;
    keywords["fallthrough"] = TokenType::FALLTHROUGH;
    keywords["struct"] = TokenType::STRUCT;
    keywords["impl"] = TokenType::IMPL;
    keywords["trait"] = TokenType::TRAIT;
    keywords["enum"] = TokenType::ENUM;
    keywords["import"] = TokenType::IMPORT;
    keywords["none"] = TokenType::NONE;
    keywords["true"] = TokenType::TRUE;
    keywords["false"] = TokenType::FALSE;
    keywords["self"] = TokenType::SELF;
    keywords["array"] = TokenType::ARRAY;
    keywords["int"] = TokenType::INT;
    keywords["uint"] = TokenType::UINT;
    keywords["uint8"] = TokenType::UINT8;
    keywords["uint16"] = TokenType::UINT16;
    keywords["uint32"] = TokenType::UINT32;
    keywords["uint64"] = TokenType::UINT64;
    keywords["int8"] = TokenType::INT8;
    keywords["int16"] = TokenType::INT16;
    keywords["int32"] = TokenType::INT32;
    keywords["int64"] = TokenType::INT64;
    keywords["char"] = TokenType::CHAR;
    keywords["double"] = TokenType::DOUBLE;
    keywords["float"] = TokenType::FLOAT;
    keywords["long"] = TokenType::LONG_DOUBLE;
    keywords["void"] = TokenType::VOID;
    keywords["bool"] = TokenType::BOOL;
    keywords["string"] = TokenType::STRING_TYPE;
    keywords["result"] = TokenType::RESULT;
    keywords["option"] = TokenType::OPTION;
    keywords["public"] = TokenType::PUBLIC;
    keywords["private"] = TokenType::PRIVATE;
    keywords["pass"] = TokenType::PASS;
    keywords["fail"] = TokenType::FAIL;
    keywords["is_pass"] = TokenType::IS_PASS;
    keywords["is_fail"] = TokenType::IS_FAIL;
    keywords["type_cast"] = TokenType::TYPE_CAST;
    keywords["operator"] = TokenType::OPERATOR;
    keywords["reflect"] = TokenType::REFLECT;
    keywords["meta"] = TokenType::META;
    keywords["util"] = TokenType::UTIL;
}

Lexer::Lexer(const std::string& source) : source(source) {
    initializeKeywords();
}

std::vector<Token> Lexer::scanTokens() {
    while (current < source.length()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", std::monostate{}, line});
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case '?': addToken(TokenType::QUESTION); break;
        case ':': addToken(TokenType::COLON); break;
        case '&': addToken(TokenType::AMPERSAND); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && current < source.length()) advance();
            } else if (match('*')) {
                while (peek() != '*' || peekNext() != '/') {
                    if (peek() == '\n') line++;
                    if (current + 2 >= source.length()) {
                        std::cout << "Line " << line << ": Unterminated multi-line comment." << std::endl;
                        // To prevent infinite loop in case of unterminated comment at EOF
                        current = source.length();
                        return;
                    }
                    advance();
                }
                advance(); // consume the '*'
                advance(); // consume the '/'
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
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                std::cout << "Line " << line << ": Unexpected character: " << c << std::endl;
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, std::monostate{});
}

void Lexer::addToken(TokenType type, const std::variant<std::monostate, std::string, double, bool>& literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, literal, line});
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
        std::cout << "Line " << line << ": Unterminated string." << std::endl;
        return;
    }
    advance();
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::number() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }
    addToken(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords[text] : TokenType::IDENTIFIER;

    if (type == TokenType::TRUE) {
        addToken(TokenType::TRUE, true);
    } else if (type == TokenType::FALSE) {
        addToken(TokenType::FALSE, false);
    } else {
        addToken(type);
    }
}

} // namespace chtholly
