#include <gtest/gtest.h>
#include "Parser.h"
#include "CodeGen.h"

void checkParserErrors(Chtholly::Parser& p) {
    const auto& errors = p.errors();
    if (errors.empty()) {
        return;
    }

    std::cerr << "Parser has " << errors.size() << " errors" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "parser error: " << msg << std::endl;
    }
    FAIL();
}

TEST(CodeGen, TestGenericStructStatement) {
    std::string input = "struct Point<T> { x: T, y: T }";
    std::string expected = "template<typename T>\nstruct Point {\npublic: T x;\npublic: T y;\n};\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestFilesystemModule) {
    std::string input = R"(
        import filesystem;
        let content = "Hello, Filesystem!";
        filesystem::write_file("test.txt", content);
        let read_content = filesystem::read_file("test.txt");
    )";
    std::string expected = "#include <filesystem>\n#include <fstream>\n\nnamespace filesystem {\n    bool exists(const std::string& path) { return std::filesystem::exists(path); }\n    bool is_file(const std::string& path) { return std::filesystem::is_regular_file(path); }\n    bool is_directory(const std::string& path) { return std::filesystem::is_directory(path); }\n    std::string read_file(const std::string& path) {\n        std::ifstream file(path);\n        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());\n        return content;\n    }\n    bool write_file(const std::string& path, const std::string& content) {\n        std::ofstream file(path);\n        file << content;\n        return file.good();\n    }\n}\n\nconst auto content = \"Hello, Filesystem!\";\nfilesystem::write_file(\"test.txt\", content);\nconst auto read_content = filesystem::read_file(\"test.txt\");\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestEnumStatement) {
    std::string input = "enum Color { Red, Green, Blue }";
    std::string expected = "enum class Color {\n    Red,\n    Green,\n    Blue\n};\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestGenericStructEndToEnd) {
    std::string input = R"(
        struct Point<T> {
            x: T,
            y: T
        }

        let p: Point<int> = Point<int>{x: 0, y: 0};
        p.x;
    )";
    std::string expected = "template<typename T>\nstruct Point {\npublic: T x;\npublic: T y;\n};\nconst Point<int> p = Point<int>{0, 0};\np.x;\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestMemberAccessExpression) {
    std::string input = "p.x;";
    std::string expected = "p.x;\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestGenericStructInstantiation) {
    std::string input = "struct Point<T> { x: T, y: T }\nlet p: Point<int> = Point<int>{x: 0, y: 0};";
    std::string expected = "template<typename T>\nstruct Point {\npublic: T x;\npublic: T y;\n};\nconst Point<int> p = Point<int>{0, 0};\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}
