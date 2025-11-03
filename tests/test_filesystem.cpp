#include "gtest/gtest.h"
#include "../src/Transpiler.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Error.h"

TEST(FileSystemTest, ReadFile) {
    std::string source = "import filesystem; let content = fs_read(\"test.txt\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <fstream>\n"
        "#include <sstream>\n\n"
        "std::string chtholly_fs_read(const std::string& path) {\n"
        "    std::ifstream file(path);\n"
        "    if (!file.is_open()) return \"\";\n"
        "    std::stringstream buffer;\n"
        "    buffer << file.rdbuf();\n"
        "    return buffer.str();\n"
        "}\n\n"
        "void chtholly_fs_write(const std::string& path, const std::string& content) {\n"
        "    std::ofstream file(path);\n"
        "    file << content;\n"
        "}\n\n"
        "int main() {\n"
        "    const auto content = chtholly_fs_read(\"test.txt\");\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(FileSystemTest, WriteFile) {
    std::string source = "import filesystem; fs_write(\"test.txt\", \"hello\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <fstream>\n"
        "#include <sstream>\n\n"
        "std::string chtholly_fs_read(const std::string& path) {\n"
        "    std::ifstream file(path);\n"
        "    if (!file.is_open()) return \"\";\n"
        "    std::stringstream buffer;\n"
        "    buffer << file.rdbuf();\n"
        "    return buffer.str();\n"
        "}\n\n"
        "void chtholly_fs_write(const std::string& path, const std::string& content) {\n"
        "    std::ofstream file(path);\n"
        "    file << content;\n"
        "}\n\n"
        "int main() {\n"
        "    chtholly_fs_write(\"test.txt\", \"hello\");\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(FileSystemTest, MissingImport) {
    std::string source = "let content = fs_read(\"test.txt\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    ErrorReporter::hadError = false;
    transpiler.transpile(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}
