#include "Lexer.h"
#include <unordered_map>

namespace chtholly {

static const std::unordered_map<std::string, TokenType> keywords = {
    {"func", TokenType::Func},
    {"let", TokenType::Let},
    {"mut", TokenType::Mut},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"return", TokenType::Return},
    {"struct", TokenType::Struct},
    {"import", TokenType::Import},
    {"public", TokenType::Public},
    {"private", TokenType::Private},
    {"self", TokenType::Self},
    {"impl", TokenType::Impl},
    {"trait", TokenType::Trait},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"none", TokenType::None},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"default", TokenType::Default},
    {"fallthrough", TokenType::Fallthrough},
    {"break", TokenType::Break},
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
    while (current < source.length()) {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::EndOfFile, "", std::monostate{}, line);
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LeftParen); break;
        case ')': addToken(TokenType::RightParen); break;
        case '{': addToken(TokenType::LeftBrace); break;
        case '}': addToken(TokenType::RightBrace); break;
        case '[': addToken(TokenType::LeftBracket); break;
        case ']': addToken(TokenType::RightBracket); break;
        case ',': addToken(TokenType::Comma); break;
        case '.': addToken(TokenType::Dot); break;
        case ';': addToken(TokenType::Semicolon); break;
        case ':': addToken(TokenType::Colon); break;
        case '?': addToken(TokenType::Question); break;
        case '+':
            if (match('+')) { addToken(TokenType::PlusPlus); }
            else if (match('=')) { addToken(TokenType::PlusEqual); }
            else { addToken(TokenType::Plus); }
            break;
        case '-':
            if (match('>')) { addToken(TokenType::Arrow); }
            else if (match('-')) { addToken(TokenType::MinusMinus); }
            else if (match('=')) { addToken(TokenType::MinusEqual); }
            else { addToken(TokenType::Minus); }
            break;
        case '*':
             if (match('=')) { addToken(TokenType::StarEqual); }
             else { addToken(TokenType::Star); }
            break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && current < source.length()) advance();
            } else if (match('*')) {
                blockComment();
            }
            else if (match('=')) { addToken(TokenType::SlashEqual); }
            else { addToken(TokenType::Slash); }
            break;
        case '%':
            if (match('=')) { addToken(TokenType::ModEqual); }
            else { addToken(TokenType::Mod); }
            break;
        case '!':
            addToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
            break;
        case '=':
            addToken(match('=') ? TokenType::EqualEqual : (match('>') ? TokenType::FatArrow : TokenType::Equal));
            break;
        case '<':
            addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
            break;
        case '>':
            addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
            break;
        case '&':
             if (match('&')) { addToken(TokenType::AmpAmp); }
             else { addToken(TokenType::Amp); }
            break;
        case '|':
             if (match('|')) { addToken(TokenType::PipePipe); }
             else { addToken(TokenType::Pipe); }
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
                // Error: Unexpected character
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

void Lexer::addToken(TokenType type, const std::variant<std::monostate, int, double, std::string, char>& literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, std::move(text), literal, line);
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
        // Error: Unterminated string.
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::String, value);
}

void Lexer::number() {
    bool isDouble = false;
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        isDouble = true;
        // Consume the "."
        advance();
        while (isdigit(peek())) advance();
    }

    if (isDouble) {
        addToken(TokenType::Double, std::stod(source.substr(start, current - start)));
    } else {
        addToken(TokenType::Integer, std::stoi(source.substr(start, current - start)));
    }
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type;
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second;
    } else {
        type = TokenType::Identifier;
    }
    addToken(type);
}

void Lexer::blockComment() {
    int nesting = 1;
    while (nesting > 0 && current < source.length()) {
        if (peek() == '/' && peekNext() == '*') {
            advance();
            advance();
            nesting++;
        } else if (peek() == '*' && peekNext() == '/') {
            advance();
            advance();
            nesting--;
        } else {
            if (peek() == '\n') line++;
            advance();
        }
    }
}

} // namespace chtholly
