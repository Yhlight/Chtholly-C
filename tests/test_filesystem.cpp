#include "gtest/gtest.h"
#include "../src/Transpiler.h"
#include "../src/Lexer.h"
#include "../src/PrattParser.h"
#include "../src/Resolver.h"
#include "../src/Error.h"
#include <fstream>

class FileSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(FileSystemTest, ReadFileWithoutImport) {
    std::string source = "let content = fs_read(\"test.txt\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(FileSystemTest, WriteFileWithoutImport) {
    std::string source = "fs_write(\"test.txt\", \"hello\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(FileSystemTest, ReadFileWithImport) {
    std::string source = "import filesystem; let content = fs_read(\"test.txt\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(FileSystemTest, WriteFileWithImport) {
    std::string source = "import filesystem; fs_write(\"test.txt\", \"hello\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(FileSystemTest, ReadExistingFile) {
    std::ofstream outfile("test.txt");
    outfile << "hello";
    outfile.close();

    std::string source = "import filesystem; let content = fs_read(\"test.txt\");";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <optional>\n\n"
        "#include <fstream>\n"
        "#include <sstream>\n\n"
        "std::optional<std::string> chtholly_fs_read(const std::string& path) {\n"
        "    std::ifstream file(path);\n"
        "    if (!file.is_open()) return std::nullopt;\n"
        "    std::stringstream buffer;\n"
        "    buffer << file.rdbuf();\n"
        "    return buffer.str();\n"
        "}\n\n"
        "bool chtholly_fs_write(const std::string& path, const std::string& content) {\n"
        "    std::ofstream file(path);\n"
        "    if (!file.is_open()) return false;\n"
        "    file << content;\n"
        "    return true;\n"
        "}\n\n"
        "int main() {\n"
        "    const auto content = chtholly_fs_read(\"test.txt\");\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST_F(FileSystemTest, ReadNonExistingFile) {
    std::string source = "import filesystem; let content = fs_read(\"nonexistent.txt\");";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <optional>\n\n"
        "#include <fstream>\n"
        "#include <sstream>\n\n"
        "std::optional<std::string> chtholly_fs_read(const std::string& path) {\n"
        "    std::ifstream file(path);\n"
        "    if (!file.is_open()) return std::nullopt;\n"
        "    std::stringstream buffer;\n"
        "    buffer << file.rdbuf();\n"
        "    return buffer.str();\n"
        "}\n\n"
        "bool chtholly_fs_write(const std::string& path, const std::string& content) {\n"
        "    std::ofstream file(path);\n"
        "    if (!file.is_open()) return false;\n"
        "    file << content;\n"
        "    return true;\n"
        "}\n\n"
        "int main() {\n"
        "    const auto content = chtholly_fs_read(\"nonexistent.txt\");\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST_F(FileSystemTest, WriteFile) {
    std::string source = "import filesystem; let result = fs_write(\"test.txt\", \"hello\");";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <optional>\n\n"
        "#include <fstream>\n"
        "#include <sstream>\n\n"
        "std::optional<std::string> chtholly_fs_read(const std::string& path) {\n"
        "    std::ifstream file(path);\n"
        "    if (!file.is_open()) return std::nullopt;\n"
        "    std::stringstream buffer;\n"
        "    buffer << file.rdbuf();\n"
        "    return buffer.str();\n"
        "}\n\n"
        "bool chtholly_fs_write(const std::string& path, const std::string& content) {\n"
        "    std::ofstream file(path);\n"
        "    if (!file.is_open()) return false;\n"
        "    file << content;\n"
        "    return true;\n"
        "}\n\n"
        "int main() {\n"
        "    const auto result = chtholly_fs_write(\"test.txt\", \"hello\");\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
