#include "Lexer.h"
#include <stdexcept>

namespace Chtholly {

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"enum", TokenType::Enum},
    {"func", TokenType::Func},
    {"let", TokenType::Let},
    {"mut", TokenType::Mut},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"return", TokenType::Return},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"struct", TokenType::Struct},
    {"import", TokenType::Import},
    {"public", TokenType::Public},
    {"private", TokenType::Private},
    {"self", TokenType::Self},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"fallthrough", TokenType::Fallthrough},
};

Lexer::Lexer(const std::string& input)
    : m_input(input), m_position(0), m_readPosition(0), m_ch(0), m_line(1), m_col(0) {
    readChar();
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_ch = 0; // EOF
    } else {
        m_ch = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition++;
    if (m_ch == '\n') {
        m_line++;
        m_col = 0;
    } else {
        m_col++;
    }
}

char Lexer::peekChar() {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (m_ch == ' ' || m_ch == '\t' || m_ch == '\n' || m_ch == '\r') {
        readChar();
    }
}

bool Lexer::isLetter(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

bool Lexer::isDigit(char ch) {
    return '0' <= ch && ch <= '9';
}

std::string Lexer::readIdentifier() {
    std::string ident;
    while (isLetter(m_ch)) {
        ident += m_ch;
        readChar();
    }
    return ident;
}

std::string Lexer::readNumber() {
    std::string num;
    while (isDigit(m_ch) || m_ch == '.') {
        num += m_ch;
        readChar();
    }
    return num;
}

std::string Lexer::readString() {
    std::string str;
    readChar(); // consume the opening '"'
    while (m_ch != '"' && m_ch != 0) {
        str += m_ch;
        readChar();
    }
    return str;
}

std::string Lexer::readCharLiteral() {
    readChar(); // consume the opening '''
    std::string ch;
    if (m_ch != 0) {
        ch += m_ch;
    }
    readChar(); // consume the character
    if (m_ch != '\'') {
        throw std::runtime_error("Unterminated character literal");
    }
    readChar(); // consume the closing '''
    return ch;
}

Token Lexer::nextToken() {
    skipWhitespace();

    Token tok;
    tok.line = m_line;
    tok.col = m_col;

    switch (m_ch) {
    case '=':
        if (peekChar() == '=') {
            readChar();
            tok = {TokenType::Equal, "==", tok.line, tok.col};
        } else {
            tok = {TokenType::Assign, "=", tok.line, tok.col};
        }
        break;
    case '+':
        tok = {TokenType::Plus, "+", tok.line, tok.col};
        break;
    case '-':
        if (peekChar() == '>') {
            readChar();
            tok = {TokenType::Arrow, "->", tok.line, tok.col};
        } else {
            tok = {TokenType::Minus, "-", tok.line, tok.col};
        }
        break;
    case '!':
        if (peekChar() == '=') {
            readChar();
            tok = {TokenType::NotEqual, "!=", tok.line, tok.col};
        } else {
            tok = {TokenType::Bang, "!", tok.line, tok.col};
        }
        break;
    case '/':
        tok = {TokenType::Slash, "/", tok.line, tok.col};
        break;
    case '*':
        tok = {TokenType::Asterisk, "*", tok.line, tok.col};
        break;
    case '%':
        tok = {TokenType::Mod, "%", tok.line, tok.col};
        break;
    case '<':
        if (peekChar() == '=') {
            readChar();
            tok = {TokenType::LessThanEqual, "<=", tok.line, tok.col};
        } else {
            tok = {TokenType::LessThan, "<", tok.line, tok.col};
        }
        break;
    case '>':
        if (peekChar() == '=') {
            readChar();
            tok = {TokenType::GreaterThanEqual, ">=", tok.line, tok.col};
        } else {
            tok = {TokenType::GreaterThan, ">", tok.line, tok.col};
        }
        break;
    case '&':
        if (peekChar() == '&') {
            readChar();
            tok = {TokenType::And, "&&", tok.line, tok.col};
        } else {
            tok = {TokenType::Illegal, "&", tok.line, tok.col};
        }
        break;
    case '|':
        if (peekChar() == '|') {
            readChar();
            tok = {TokenType::Or, "||", tok.line, tok.col};
        } else {
            tok = {TokenType::Illegal, "|", tok.line, tok.col};
        }
        break;
    case ';':
        tok = {TokenType::Semicolon, ";", tok.line, tok.col};
        break;
    case ':':
        tok = {TokenType::Colon, ":", tok.line, tok.col};
        break;
    case '.':
        tok = {TokenType::Dot, ".", tok.line, tok.col};
        break;
    case '(':
        tok = {TokenType::LParen, "(", tok.line, tok.col};
        break;
    case ')':
        tok = {TokenType::RParen, ")", tok.line, tok.col};
        break;
    case ',':
        tok = {TokenType::Comma, ",", tok.line, tok.col};
        break;
    case '{':
        tok = {TokenType::LBrace, "{", tok.line, tok.col};
        break;
    case '}':
        tok = {TokenType::RBrace, "}", tok.line, tok.col};
        break;
    case '[':
        tok = {TokenType::LBracket, "[", tok.line, tok.col};
        break;
    case ']':
        tok = {TokenType::RBracket, "]", tok.line, tok.col};
        break;
    case '"':
        tok.type = TokenType::String;
        tok.literal = readString();
        readChar(); // consume the closing '"'
        return tok;
    case '\'':
        tok.type = TokenType::Char;
        tok.literal = readCharLiteral();
        return tok;
    case 0:
        tok = {TokenType::Eof, "", tok.line, tok.col};
        return tok;
    default:
        if (isLetter(m_ch)) {
            tok.literal = readIdentifier();
            auto it = keywords.find(tok.literal);
            if (it != keywords.end()) {
                tok.type = it->second;
            } else {
                tok.type = TokenType::Identifier;
            }
            return tok;
        } else if (isDigit(m_ch) || m_ch == '.') {
            tok.literal = readNumber();
            if (tok.literal.find('.') != std::string::npos) {
                tok.type = TokenType::Double;
            } else {
                tok.type = TokenType::Int;
            }
            return tok;
        } else {
            tok = {TokenType::Illegal, std::string(1, m_ch), tok.line, tok.col};
        }
    }

    readChar();
    return tok;
}

} // namespace Chtholly
