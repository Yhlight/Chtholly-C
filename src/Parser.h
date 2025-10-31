#pragma once

#include "AST.h"
#include "Token.h"
#include <vector>
#include <memory>

namespace chtholly {

class Parser {
public:
    Parser(std::vector<Token> tokens);
    std::unique_ptr<BlockStmtAST> parse();

private:
    std::unique_ptr<StmtAST> parseStatement();
    std::unique_ptr<StmtAST> parseVarDecl();
    std::unique_ptr<FuncDeclAST> parseFuncDecl();
    std::unique_ptr<StmtAST> parseIfStmt();
    std::unique_ptr<StmtAST> parseSwitchStmt();
    std::unique_ptr<StmtAST> parseEnumDecl();
    std::unique_ptr<StmtAST> parseWhileStmt();
    std::unique_ptr<StmtAST> parseForStmt();
    std::unique_ptr<StmtAST> parseImportStmt();
    std::unique_ptr<BlockStmtAST> parseBlock();
    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<ExprAST> parseAssignment();
    std::unique_ptr<ExprAST> parseComparison();
    std::unique_ptr<ExprAST> parseTerm();
    std::unique_ptr<ExprAST> parseFactor();
    std::unique_ptr<ExprAST> parseUnary();
    std::unique_ptr<ExprAST> parseCall();
    std::unique_ptr<ExprAST> parsePrimary();

    Token& peek(int k = 0);
    Token& advance();
    bool match(const std::vector<TokenType>& types);
    Token& consume(TokenType type, const std::string& message);
    bool isAtEnd();

    std::vector<Token> tokens;
    int current = 0;
};

} // namespace chtholly
