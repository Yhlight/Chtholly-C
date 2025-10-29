#include "Lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

char Lexer::get_char() {
    if (position_ < source_.length()) {
        return source_[position_++];
    }
    return EOF;
}

Token Lexer::get_token() {
    // Skip whitespace
    while (position_ < source_.length() && isspace(source_[position_])) {
        position_++;
    }

    if (position_ >= source_.length()) {
        return Token::Eof;
    }

    char current_char = source_[position_];

    // Identifiers and keywords
    if (isalpha(current_char) || current_char == '_') {
        identifier_ = "";
        while (position_ < source_.length() && (isalnum(source_[position_]) || source_[position_] == '_')) {
            identifier_ += source_[position_++];
        }

        if (identifier_ == "func") return Token::Func;
        if (identifier_ == "let") return Token::Let;
        if (identifier_ == "mut") return Token::Mut;
        if (identifier_ == "if") return Token::If;
        if (identifier_ == "else") return Token::Else;
        if (identifier_ == "struct") return Token::Struct;
        if (identifier_ == "while") return Token::While;
        if (identifier_ == "import") return Token::Import;
        return Token::Identifier;
    }

    // Numbers
    if (isdigit(current_char) || current_char == '.') {
        std::string num_str;
        while (position_ < source_.length() && (isdigit(source_[position_]) || source_[position_] == '.')) {
            num_str += source_[position_++];
        }
        number_ = std::stod(num_str);
        return Token::Number;
    }

    // Comments
    if (current_char == '/') {
        if (position_ + 1 < source_.length() && source_[position_ + 1] == '/') {
            // Skip to the end of the line
            while (position_ < source_.length() && source_[position_] != '\n' && source_[position_] != '\r') {
                position_++;
            }
            return get_token(); // Get the next token
        }
    }

    position_++;
    switch (current_char) {
        case '=':
        case '<':
        case '>':
        case '+':
        case '-':
        case '*':
        case '/':
            operator_ = current_char;
            return Token::Operator;
        case ';': return Token::Semicolon;
        case '(': return Token::LeftParen;
        case ')': return Token::RightParen;
        case '{': return Token::LeftBrace;
        case '}': return Token::RightBrace;
        case '[': return Token::LeftBracket;
        case ']': return Token::RightBracket;
        case ',': return Token::Comma;
        case ':': return Token::Colon;
    }

    return Token::Unknown;
}

std::string to_string(Token token) {
    switch (token) {
        case Token::Eof: return "Eof";
        case Token::Func: return "Func";
        case Token::Let: return "Let";
        case Token::Mut: return "Mut";
        case Token::If: return "If";
        case Token::Else: return "Else";
        case Token::Struct: return "Struct";
        case Token::While: return "While";
        case Token::Import: return "Import";
        case Token::Identifier: return "Identifier";
        case Token::Number: return "Number";
        case Token::Operator: return "Operator";
        case Token::Semicolon: return "Semicolon";
        case Token::LeftParen: return "LeftParen";
        case Token::RightParen: return "RightParen";
        case Token::LeftBrace: return "LeftBrace";
        case Token::RightBrace: return "RightBrace";
        case Token::LeftBracket: return "LeftBracket";
        case Token::RightBracket: return "RightBracket";
        case Token::Comma: return "Comma";
        case Token::Colon: return "Colon";
        case Token::Unknown: return "Unknown";
        default:
            throw std::runtime_error("Unknown token");
    }
}
