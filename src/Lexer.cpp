#include "Lexer.h"
#include "Error.h"

#include <map>

std::map<std::string, TokenType> keywords = {
    {"let",    TokenType::LET},
    {"mut",    TokenType::MUT},
    {"true",   TokenType::TRUE},
    {"false",  TokenType::FALSE},
    {"if",     TokenType::IF},
    {"else",   TokenType::ELSE},
    {"while",  TokenType::WHILE},
    {"func",   TokenType::FUNC},
    {"function", TokenType::FUNCTION},
    {"return", TokenType::RETURN},
    {"struct", TokenType::STRUCT},
    {"print",  TokenType::PRINT},
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", std::monostate{}, line});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
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
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ':': addToken(TokenType::COLON); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
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
        case '&':
            if (match('&')) {
                addToken(TokenType::AMPERSAND_AMPERSAND);
            } else {
                addToken(TokenType::AMPERSAND);
            }
            break;
        case '|':
            if (match('|')) {
                addToken(TokenType::PIPE_PIPE);
            }
            break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (source[current] != '\n' && !isAtEnd()) advance();
            } else {
                addToken(TokenType::SLASH);
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
                ErrorReporter::error(line, "Unexpected character.");
            }
            break;
    }
}

void Lexer::string() {
    while (source[current] != '"' && !isAtEnd()) {
        if (source[current] == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        ErrorReporter::error(line, "Unterminated string.");
        return;
    }

    advance(); // The closing ".

    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::number() {
    while (isdigit(source[current])) advance();

    if (source[current] == '.' && isdigit(source[current + 1])) {
        advance();
        while (isdigit(source[current])) advance();
    }

    addToken(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}

void Lexer::identifier() {
    while (isalnum(source[current]) || source[current] == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords.at(text) : TokenType::IDENTIFIER;
    addToken(type);
}
