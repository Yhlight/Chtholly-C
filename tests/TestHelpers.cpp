#include "TestHelpers.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Sema.h"
#include "../src/CodeGen.h"
#include <regex>
#include <iostream>

std::string compile(const std::string& source) {
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();

    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();

    chtholly::Sema sema;
    sema.analyze(statements);
    if (sema.hadError()) {
        return "Sema error";
    }

    chtholly::CodeGen codegen;
    return codegen.generate(statements);
}

std::string normalize(const std::string& s) {
    std::string result = std::regex_replace(s, std::regex("^\\s+"), "");
    result = std::regex_replace(result, std::regex("\\s+$"), "");
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    return result;
}
