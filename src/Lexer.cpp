#include "Lexer.h"
#include <unordered_map>
#include "Error.h"

static std::unordered_map<std::string, TokenType> keywords;

static void initializeKeywords() {
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["func"] = TokenType::FUNC;
    keywords["result"] = TokenType::RESULT;
    keywords["pass"] = TokenType::PASS;
    keywords["fail"] = TokenType::FAIL;
    keywords["is_pass"] = TokenType::IS_PASS;
    keywords["is_fail"] = TokenType::IS_FAIL;
    keywords["let"] = TokenType::LET;
    keywords["mut"] = TokenType::MUT;
    keywords["option"] = TokenType::OPTION;
    keywords["none"] = TokenType::NONE;
    keywords["unwarp"] = TokenType::UNWARP;
    keywords["unwarp_or"] = TokenType::UNWARP_OR;
    keywords["int"] = TokenType::INT;
    keywords["int8"] = TokenType::INT8;
    keywords["int16"] = TokenType::INT16;
    keywords["int32"] = TokenType::INT32;
    keywords["int64"] = TokenType::INT64;
    keywords["uint8"] = TokenType::UINT8;
    keywords["uint16"] = TokenType::UINT16;
    keywords["uint32"] = TokenType::UINT32;
    keywords["uint64"] = TokenType::UINT64;
    keywords["char"] = TokenType::CHAR_TYPE;
    keywords["double"] = TokenType::DOUBLE;
    keywords["float"] = TokenType::FLOAT;
    keywords["long"] = TokenType::LONG;
    keywords["void"] = TokenType::VOID;
    keywords["bool"] = TokenType::BOOL;
    keywords["string"] = TokenType::STRING_TYPE;
    keywords["array"] = TokenType::ARRAY;
    keywords["struct"] = TokenType::STRUCT;
    keywords["enum"] = TokenType::ENUM;
    keywords["return"] = TokenType::RETURN;
    keywords["private"] = TokenType::PRIVATE;
    keywords["public"] = TokenType::PUBLIC;
    keywords["self"] = TokenType::SELF;
    keywords["switch"] = TokenType::SWITCH;
    keywords["case"] = TokenType::CASE;
    keywords["break"] = TokenType::BREAK;
    keywords["fallthrough"] = TokenType::FALLTHROUGH;
    keywords["trait"] = TokenType::TRAIT;
    keywords["impl"] = TokenType::IMPL;
    keywords["import"] = TokenType::IMPORT;
    keywords["type_cast"] = TokenType::TYPE_CAST;
}

Lexer::Lexer(const std::string& source) : source_(source) {
    initializeKeywords();
}

std::vector<Token> Lexer::scanTokens() {
    while (current_ < source_.length()) {
        start_ = current_;
        scanToken();
    }

    tokens_.emplace_back(TokenType::END_OF_FILE, "", line_);
    return tokens_;
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
        case '-':
            if (match('>')) {
                addToken(TokenType::ARROW);
            } else {
                addToken(TokenType::MINUS);
            }
            break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '&': addToken(TokenType::AMPERSAND); break;
        case ':':
            if (match(':')) {
                addToken(TokenType::COLON_COLON);
            } else {
                addToken(TokenType::COLON);
            }
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
                // A comment goes until the end of the line.
                while (peek() != '\n' && current_ < source_.length()) advance();
            } else if (match('*')) {
                // A block comment goes until the closing */.
                while (peek() != '*' && peekNext() != '/' && current_ < source_.length()) {
                    if (peek() == '\n') line_++;
                    advance();
                }
                if (current_ >= source_.length()) {
                    ErrorReporter::report(line_, "Unterminated block comment.");
                    return;
                }
                advance(); // consume the *
                advance(); // consume the /
            }
            else {
                addToken(TokenType::SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            line_++;
            break;
        case '"': string(); break;
        case '\'':
            // Character literal
            if (peekNext() == '\'') {
                advance(); // consume the character
                char value = source_[current_ - 1];
                advance(); // consume the closing '
                addToken(TokenType::CHAR, std::string(1, value));
            } else {
                ErrorReporter::report(line_, "Unterminated character literal.");
            }
            break;
        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                ErrorReporter::report(line_, "Unexpected character.");
            }
            break;
    }
}

char Lexer::advance() {
    return source_[current_++];
}

void Lexer::addToken(TokenType type) {
    std::string text = source_.substr(start_, current_ - start_);
    tokens_.emplace_back(type, text, line_);
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    tokens_.emplace_back(type, literal, line_);
}

bool Lexer::match(char expected) {
    if (current_ >= source_.length()) return false;
    if (source_[current_] != expected) return false;

    current_++;
    return true;
}

char Lexer::peek() {
    if (current_ >= source_.length()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

void Lexer::string() {
    while (peek() != '"' && current_ < source_.length()) {
        if (peek() == '\n') line_++;
        advance();
    }

    if (current_ >= source_.length()) {
        ErrorReporter::report(line_, "Unterminated string.");
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();
    }

    addToken(TokenType::NUMBER, source_.substr(start_, current_ - start_));
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source_.substr(start_, current_ - start_);
    TokenType type;
    if (keywords.count(text)) {
        type = keywords.at(text);
    } else {
        type = TokenType::IDENTIFIER;
    }
    addToken(type);
}
