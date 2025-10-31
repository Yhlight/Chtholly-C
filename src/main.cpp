#include <iostream>
#include <vector>
#include "Lexer.h"
#include "Token.h"

// Function to convert TokenType to string for printing
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::MINUS: return "MINUS";
        case TokenType::PLUS: return "PLUS";
        case TokenType::SLASH: return "SLASH";
        case TokenType::STAR: return "STAR";
        case TokenType::MODULO: return "MODULO";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::AMPERSAND_AMPERSAND: return "AMPERSAND_AMPERSAND";
        case TokenType::PIPE: return "PIPE";
        case TokenType::PIPE_PIPE: return "PIPE_PIPE";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::CHAR: return "CHAR";
        case TokenType::FUNC: return "FUNC";
        case TokenType::LET: return "LET";
        case TokenType::MUT: return "MUT";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::SWITCH: return "SWITCH";
        case TokenType::CASE: return "CASE";
        case TokenType::FALLTHROUGH: return "FALLTHROUGH";
        case TokenType::FOR: return "FOR";
        case TokenType::WHILE: return "WHILE";
        case TokenType::RETURN: return "RETURN";
        case TokenType::STRUCT: return "STRUCT";
        case TokenType::ENUM: return "ENUM";
        case TokenType::PUBLIC: return "PUBLIC";
        case TokenType::PRIVATE: return "PRIVATE";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::TRAIT: return "TRAIT";
        case TokenType::IMPL: return "IMPL";
        case TokenType::OPTION: return "OPTION";
        case TokenType::NONE: return "NONE";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

int main() {
    std::string source = R"(
        // This is a comment
        func main() {
            let x = 10;
            let y = "hello";
            if (x > 5) {
                return;
            }
        }
    )";

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        for (const auto& token : tokens) {
            std::cout << "Type: " << tokenTypeToString(token.type)
                      << ", Lexeme: '" << token.lexeme
                      << "', Line: " << token.line << std::endl;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
