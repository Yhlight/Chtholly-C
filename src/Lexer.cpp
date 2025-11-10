#include "Lexer.h"
#include <utility>
#include <iostream>
#include <cctype>

namespace chtholly {

const std::map<std::string, TokenType> Lexer::keywords = {
    {"if", TokenType::IF}, {"else", TokenType::ELSE}, {"for", TokenType::FOR}, {"while", TokenType::WHILE},
    {"switch", TokenType::SWITCH}, {"case", TokenType::CASE}, {"default", TokenType::DEFAULT}, {"break", TokenType::BREAK}, {"fallthrough", TokenType::FALLTHROUGH},
    {"func", TokenType::FUNC}, {"return", TokenType::RETURN}, {"struct", TokenType::STRUCT}, {"enum", TokenType::ENUM},
    {"impl", TokenType::IMPL}, {"trait", TokenType::TRAIT}, {"let", TokenType::LET}, {"mut", TokenType::MUT},
    {"public", TokenType::PUBLIC}, {"private", TokenType::PRIVATE}, {"self", TokenType::SELF}, {"none", TokenType::NONE},
    {"true", TokenType::TRUE}, {"false", TokenType::FALSE}, {"import", TokenType::IMPORT},
    {"print", TokenType::PRINT}, {"input", TokenType::INPUT},
    {"type_cast", TokenType::TYPE_CAST},
    {"iostream", TokenType::IOSTREAM},
    {"filesystem", TokenType::FILESYSTEM}, {"fs_read", TokenType::FS_READ}, {"fs_write", TokenType::FS_WRITE},
    {"operator", TokenType::OPERATOR}, {"reflect", TokenType::REFLECT}, {"meta", TokenType::META}, {"util", TokenType::UTIL},
    {"int", TokenType::INT}, {"int8", TokenType::INT8}, {"int16", TokenType::INT16}, {"int32", TokenType::INT32},
    {"int64", TokenType::INT64}, {"uint", TokenType::UINT}, {"uint8", TokenType::UINT8}, {"uint16", TokenType::UINT16}, {"uint32", TokenType::UINT32},
    {"uint64", TokenType::UINT64}, {"char", TokenType::CHAR_TYPE}, {"double", TokenType::DOUBLE}, {"float", TokenType::FLOAT},
    {"long", TokenType::LONG_DOUBLE}, {"void", TokenType::VOID}, {"bool", TokenType::BOOL}, {"string", TokenType::STRING_TYPE},
    {"array", TokenType::ARRAY}, {"function", TokenType::FUNCTION}, {"option", TokenType::OPTION},
    {"os", TokenType::OS}
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", nullptr, line);
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
        case '?': addToken(TokenType::QUESTION); break;
        case '%': addToken(match('=') ? TokenType::PERCENT_EQUAL : TokenType::PERCENT); break;
        case '*':
            if (match('*')) { addToken(TokenType::STAR_STAR); }
            else { addToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR); }
            break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '&': addToken(match('&') ? TokenType::AND : TokenType::AMPERSAND); break;
        case '|': addToken(match('|') ? TokenType::OR : TokenType::PIPE); break;

        case '+':
            if (match('=')) { addToken(TokenType::PLUS_EQUAL); }
            else if (match('+')) { addToken(TokenType::PLUS_PLUS); }
            else { addToken(TokenType::PLUS); }
            break;
        case '-':
            if (match('=')) { addToken(TokenType::MINUS_EQUAL); }
            else if (match('-')) { addToken(TokenType::MINUS_MINUS); }
            else if (match('>')) { addToken(TokenType::ARROW); }
            else { addToken(TokenType::MINUS); }
            break;

        case ':':
            addToken(match(':') ? TokenType::COLON_COLON : TokenType::COLON);
            break;

        case '/':
            if (match('/')) { while (peek() != '\n' && !isAtEnd()) advance(); }
            else if (match('*')) { blockComment(); }
            else { addToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH); }
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
                std::cerr << "Line " << line << ": Unexpected character '" << c << "'" << std::endl;
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, nullptr);
}

void Lexer::addToken(TokenType type, const std::variant<std::nullptr_t, std::string, double, long long, bool, char>& literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
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
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING_LITERAL, value);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }

    std::string num_str = source.substr(start, current-start);
    if (num_str.find('.') != std::string::npos) {
        addToken(TokenType::NUMBER_LITERAL, std::stod(num_str));
    } else {
        addToken(TokenType::NUMBER_LITERAL, std::stoll(num_str));
    }
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
    addToken(type);
}

void Lexer::blockComment() {
    while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
            advance();
            advance();
            return;
        }
        if (peek() == '/' && peekNext() == '*') {
            advance();
            advance();
            blockComment(); // nested block comments
        } else {
            if (peek() == '\n') line++;
            advance();
        }
    }

    if (isAtEnd()) {
        std::cerr << "Line " << line << ": Unterminated block comment." << std::endl;
    }
}

} // namespace chtholly
