#include "TestHelpers.h"
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"
#include <algorithm>
#include <cctype>

std::string compile(const std::string& source) {
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::Transpiler transpiler;
    return transpiler.transpile(statements);
}

std::string normalize(const std::string& s) {
    std::string result = s;
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](unsigned char c) { return std::isspace(c); }),
                 result.end());
    return result;
}
