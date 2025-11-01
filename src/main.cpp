#include <iostream>
#include <vector>
#include <string>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/AstPrinter.h"
#include "../include/Token.h"

// Helper to convert TokenType to string for printing
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
        case TokenType::MINUS: return "MINUS";
        case TokenType::PLUS: return "PLUS";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::SLASH: return "SLASH";
        case TokenType::STAR: return "STAR";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::COLON: return "COLON";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::PLUS_PLUS: return "PLUS_PLUS";
        case TokenType::MINUS_MINUS: return "MINUS_MINUS";
        case TokenType::PLUS_EQUAL: return "PLUS_EQUAL";
        case TokenType::MINUS_EQUAL: return "MINUS_EQUAL";
        case TokenType::STAR_EQUAL: return "STAR_EQUAL";
        case TokenType::SLASH_EQUAL: return "SLASH_EQUAL";
        case TokenType::PERCENT_EQUAL: return "PERCENT_EQUAL";
        case TokenType::ARROW: return "ARROW";
        case TokenType::COLON_COLON: return "COLON_COLON";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::DOUBLE: return "DOUBLE";
        case TokenType::CHAR: return "CHAR";
        case TokenType::STRUCT: return "STRUCT";
        case TokenType::FUNC: return "FUNC";
        case TokenType::LET: return "LET";
        case TokenType::MUT: return "MUT";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::SWITCH: return "SWITCH";
        case TokenType::CASE: return "CASE";
        case TokenType::BREAK: return "BREAK";
        case TokenType::FALLTHROUGH: return "FALLTHROUGH";
        case TokenType::RETURN: return "RETURN";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::NONE: return "NONE";
        case TokenType::ENUM: return "ENUM";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::PUBLIC: return "PUBLIC";
        case TokenType::PRIVATE: return "PRIVATE";
        case TokenType::SELF: return "SELF";
        case TokenType::TRAIT: return "TRAIT";
        case TokenType::IMPL: return "IMPL";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

std::string Token::toString() const {
    return "Token [type: " + tokenTypeToString(type) +
           ", lexeme: '" + lexeme +
           "', line: " + std::to_string(line) + "]";
}


int main() {
    std::string source = R"(
        let a = 10;
        mut b = "hello";
        let c = a + 5;
        b = "world";
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    AstPrinter printer;
    std::cout << printer.print(statements);

    return 0;
}
