#include "TestHelpers.h"
#include "Lexer.h"
#include "Parser.h"
#include "Sema.h"
#include "CodeGen.h"
#include <string>
#include <vector>
#include <cctype>

std::string compile(const std::string& code) {
    chtholly::Lexer lexer(code);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::Sema sema;
    sema.analyze(statements);
    if (sema.hadError()) {
        return "Sema Error";
    }
    chtholly::CodeGen codegen;
    return codegen.generate(statements);
}

std::string normalize(const std::string& s) {
    std::string result;
    bool in_whitespace = false;
    for (char c : s) {
        if (std::isspace(c)) {
            if (!in_whitespace) {
                result += ' ';
                in_whitespace = true;
            }
        } else {
            result += c;
            in_whitespace = false;
        }
    }
    // Trim leading/trailing space
    size_t first = result.find_first_not_of(' ');
    if (std::string::npos == first) return "";
    size_t last = result.find_last_not_of(' ');
    return result.substr(first, (last - first + 1));
}
