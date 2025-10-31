#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "Sema.h"
#include "CodeGen.h"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

using namespace chtholly;

TEST(FilesystemTest, Exists) {
    std::string code = R"(
        import filesystem;
        func main() {
            let p = "test_file.txt";
            exists(p);
        }
    )";

    std::ofstream("test_file.txt") << "hello";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Sema sema;
    sema.analyze(*ast);
    CodeGen codegen(sema);
    std::string cppCode = codegen.generate(*ast);

    ASSERT_TRUE(std::filesystem::exists("test_file.txt"));
    std::filesystem::remove("test_file.txt");
}

TEST(FilesystemTest, IsDirectory) {
    std::string code = R"(
        import filesystem;
        func main() {
            let p = "test_dir";
            is_directory(p);
        }
    )";

    std::filesystem::create_directory("test_dir");

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Sema sema;
    sema.analyze(*ast);
    CodeGen codegen(sema);
    std::string cppCode = codegen.generate(*ast);

    ASSERT_TRUE(std::filesystem::is_directory("test_dir"));
    std::filesystem::remove("test_dir");
}
