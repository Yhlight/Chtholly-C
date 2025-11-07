#include <gtest/gtest.h>
#include "Transpiler.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <string>
#include <fstream>

using namespace chtholly;

std::string transpile(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Transpiler transpiler;
    return transpiler.transpile(statements);
}

TEST(TranspilerTest, VariableDeclaration) {
    std::string source = "let x = 10;";
    std::string expected = "const int x = 10;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, MutableVariableDeclaration) {
    std::string source = "mut y = \"hello\";";
    std::string expected = "std::string y = \"hello\";\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, ExpressionStatement) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "1 + 2 * 3;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, BlockStatement) {
    std::string source = "{ let a = 1; mut b = 2; }";
    std::string expected = "{\nconst int a = 1;\nint b = 2;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, IfStatement) {
    std::string source = "if (x > 0) { x = x - 1; }";
    std::string expected = "if (x > 0) {\nx = x - 1;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, IfElseStatement) {
    std::string source = "if (x > 0) { x = 1; } else { x = 2; }";
    std::string expected = "if (x > 0) {\nx = 1;\n}\nelse {\nx = 2;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, WhileStatement) {
    std::string source = "while (x > 0) { x = x - 1; }";
    std::string expected = "while (x > 0) {\nx = x - 1;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, ForStatement) {
    std::string source = "for (let i = 0; i < 10; i = i + 1) { }";
    std::string expected = "for (const int i = 0; i < 10; i = i + 1) {\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, OptionType) {
    std::string source = "let x: option<int> = none;";
    std::string expected = "#include <optional>\nconst std::optional<int> x = nullptr;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, OptionUnwarp) {
    std::string source = "let x: option<int> = 10; let y = x.unwarp();";
    std::string expected = "#include <optional>\nconst std::optional<int> x = 10;\nconst auto y = x.value();\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, OptionUnwarpOr) {
    std::string source = "let x: option<int> = none; let y = x.unwarp_or(5);";
    std::string expected = "#include <optional>\nconst std::optional<int> x = nullptr;\nconst int y = x.value_or(5);\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, TypeCastExpression) {
    std::string source = "let x = type_cast<int>(10);";
    std::string expected = "const int x = static_cast<int>(10);\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, SwitchStatement) {
    std::string source = "switch (x) { case 1: { y = 1; break; } default: { y = 2; } }";
    std::string expected = "{\nauto&& __switch_val = x;\nif (__switch_val == 1) {\ny = 1;\n}\nelse {\ny = 2;\n}\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, SwitchFallthroughStatement) {
    std::string source = "switch (x) { case 1: { y = 1; fallthrough; } case 2: { y = 2; } }";
    std::string expected = "{\nauto&& __switch_val = x;\nif (__switch_val == 1 || __switch_val == 2) {\ny = 1;\ny = 2;\n}\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, SwitchMultipleFallthroughStatement) {
    std::string source = "switch (x) { case 1: { y = 1; fallthrough; } case 2: { y = 2; fallthrough; } case 3: { y = 3; } }";
    std::string expected = "{\nauto&& __switch_val = x;\nif (__switch_val == 1 || __switch_val == 2 || __switch_val == 3) {\ny = 1;\ny = 2;\ny = 3;\n}\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, FunctionDeclaration) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    std::string expected = "int add(int a, int b) {\nreturn a + b;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, StructDeclaration) {
    std::string source = "struct Point { x: int; y: int; }";
    std::string expected = "struct Point {\nint x;\nint y;\ntemplate<class Archive>\nvoid serialize(Archive& archive) {\n    archive(x);\n    archive(y);\n}\n};\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, StructWithMethod) {
    std::string source = "struct Point { x: int; move(dx: int) -> void { x = x + dx; } }";
    std::string expected = "struct Point {\nint x;\ntemplate<class Archive>\nvoid serialize(Archive& archive) {\n    archive(x);\n}\nvoid move(int dx) {\nx = x + dx;\n}\n};\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, PrintStatement) {
    std::string source = "import iostream; print(\"Hello, World!\");";
    std::string expected = "#include <iostream>\nstd::cout << \"Hello, World!\" << std::endl;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, PrintWithoutImport) {
    std::string source = "print(\"Hello, World!\");";
    std::string expected = "/* ERROR: 'print' function called without importing 'iostream' */;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, FilesystemReadWrite) {
    // Create a temporary file to read from
    std::ofstream outfile("test_read.txt");
    outfile << "Hello from file!";
    outfile.close();

    std::string source = R"(
        import filesystem;
        let content = fs_read("test_read.txt");
        fs_write("test_write.txt", content);
    )";

    // We can't execute the transpiled code directly here,
    // but we can verify that the C++ output is correct.
    std::string expected_prefix = "#include <fstream>\n#include <sstream>\n";
    std::string expected_read = "fs_read(\"test_read.txt\")";
    std::string expected_write = "{ std::ofstream file(\"test_write.txt\"); file << content; }";

    std::string transpiled_code = transpile(source);

    EXPECT_TRUE(transpiled_code.rfind(expected_prefix, 0) == 0);
    EXPECT_NE(transpiled_code.find(expected_read), std::string::npos);
    EXPECT_NE(transpiled_code.find(expected_write), std::string::npos);
}

TEST(TranspilerTest, FilesystemWithoutImport) {
    std::string source = R"(
        let content = fs_read("test.txt");
        fs_write("test.txt", "hello");
    )";
    std::string expected_read_error = "/* ERROR: 'fs_read' function called without importing 'filesystem' */";
    std::string expected_write_error = "/* ERROR: 'fs_write' function called without importing 'filesystem' */";

    std::string transpiled_code = transpile(source);

    EXPECT_NE(transpiled_code.find(expected_read_error), std::string::npos);
    EXPECT_NE(transpiled_code.find(expected_write_error), std::string::npos);
}

TEST(TranspilerTest, MetaIsInt) {
    std::string source = R"(
        let x: int = 10;
        let y: string = "hello";
        let is_x_int = meta::is_int(x);
        let is_y_int = meta::is_int(y);
    )";
    std::string expected = "const int x = 10;\nconst std::string y = \"hello\";\nconst bool is_x_int = true;\nconst bool is_y_int = false;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, OperatorOverloading) {
    std::string source = R"(
        import operator;
        struct Point impl operator::add {
            x: int;
            y: int;
            add(other: Point) -> Point {
                return Point{x: self.x + other.x, y: self.y + other.y};
            }
        }
        func main() {
            let p1: Point = Point{x: 1, y: 2};
            let p2: Point = Point{x: 3, y: 4};
            let p3 = p1 + p2;
        }
    )";
    std::string expected = "const Point p3 = p1.add(p2);\n";
    std::string transpiled_code = transpile(source);
    EXPECT_NE(transpiled_code.find(expected), std::string::npos);
}

TEST(TranspilerTest, InputStatement) {
    std::string source = "import iostream; let name = input();";
    std::string expected = "const std::string name = input();\n";
    EXPECT_NE(transpile(source).find(expected), std::string::npos);
}

TEST(TranspilerTest, InputWithoutImport) {
    std::string source = "let name = input();";
    std::string expected_error = "/* ERROR: 'input' function called without importing 'iostream' */";
    std::string transpiled_code = transpile(source);
    EXPECT_NE(transpiled_code.find(expected_error), std::string::npos);
}
