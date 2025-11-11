#include <gtest/gtest.h>
#include "Transpiler.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

using namespace chtholly;

class TranspilerTest : public ::testing::Test {
protected:
    std::string transpile(const std::string& source) {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();
        Parser parser(tokens);
        auto statements = parser.parse();
        Transpiler transpiler;
        return transpiler.transpile(statements);
    }
};

TEST_F(TranspilerTest, VariableDeclaration) {
    std::string source = "let x = 10;";
    std::string expected = "const int x = 10;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, MutableVariableDeclaration) {
    std::string source = "mut y = \"hello\";";
    std::string expected = "std::string y = \"hello\";\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, ExpressionStatement) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "1 + 2 * 3;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, BlockStatement) {
    std::string source = "{ let a = 1; mut b = 2; }";
    std::string expected = "{\nconst int a = 1;\nint b = 2;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, IfStatement) {
    std::string source = "if (x > 0) { x = x - 1; }";
    std::string expected = "if (x > 0) {\nx = x - 1;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, IfElseStatement) {
    std::string source = "if (x > 0) { x = 1; } else { x = 2; }";
    std::string expected = "if (x > 0) {\nx = 1;\n}\nelse {\nx = 2;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, WhileStatement) {
    std::string source = "while (x > 0) { x = x - 1; }";
    std::string expected = "while (x > 0) {\nx = x - 1;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, ForStatement) {
    std::string source = "for (let i = 0; i < 10; i = i + 1) { }";
    std::string expected = "for (const int i = 0; i < 10; i = i + 1) {\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, OptionType) {
    std::string source = "let x: option<int> = none;";
    std::string expected = "#include <optional>\nconst std::optional<int> x = nullptr;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, OptionUnwarp) {
    std::string source = "let x: option<int> = 10; let y = x.unwarp();";
    std::string expected = "#include <optional>\nconst std::optional<int> x = 10;\nconst auto y = x.value();\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, OptionUnwarpOr) {
    std::string source = "let x: option<int> = none; let y = x.unwarp_or(5);";
    std::string expected = "#include <optional>\nconst std::optional<int> x = nullptr;\nconst int y = x.value_or(5);\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, TypeCastExpression) {
    std::string source = "let x = type_cast<int>(10);";
    std::string expected = "const int x = static_cast<int>(10);\n";
    EXPECT_EQ(transpile(source), expected);
}


TEST_F(TranspilerTest, SwitchStatement) {
    std::string source = R"(
        switch (x) {
            case 1: {
                y = 1;
                fallthrough;
            }
            case 2: {
                y = 2;
                break;
            }
            case 3: {
                y = 3;
            }
            default: {
                y = 4;
            }
        }
    )";
    std::string expected = "switch (x) {\ncase 1:\n{\ny = 1;\n[[fallthrough]];\n}\ncase 2:\n{\ny = 2;\nbreak;\n}\ncase 3:\n{\ny = 3;\n}\nbreak;\ndefault:\n{\ny = 4;\n}\nbreak;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, FunctionDeclaration) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    std::string expected = "int add(int a, int b) {\nreturn a + b;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, StructDeclaration) {
    std::string source = "struct Point { public: x: int; y: int; }";
    std::string expected = "struct Point {\npublic:\nint x;\nint y;\n};\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, StructWithMethod) {
    std::string source = "struct Point { public: x: int; public: func move(dx: int) -> void { self.x = self.x + dx; } }";
    std::string expected = "struct Point {\npublic:\nint x;\nvoid move(int dx) {\nthis->x = this->x + dx;\n}\n};\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, PrintStatement) {
    std::string source = "import iostream; print(\"Hello, World!\");";
    std::string expected = "#include <iostream>\nstd::cout << \"Hello, World!\" << std::endl;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, PrintWithoutImport) {
    std::string source = "print(\"Hello, World!\");";
    std::string expected = "/* ERROR: 'print' function called without importing 'iostream' */;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, FilesystemReadWrite) {
    // This test is more of a sanity check for transpilation,
    // the actual execution is tested in TestFilesystem.cpp
    std::string source = R"(
        import filesystem;
        let content = fs_read("test_read.txt");
        fs_write("test_write.txt", content);
    )";

    // We can't execute the transpiled code directly here,
    // but we can verify that the C++ output is correct.
    std::string expected_read = "chtholly_fs_read(\"test_read.txt\")";
    std::string expected_write = "chtholly_fs_write(\"test_write.txt\", content)";

    std::string transpiled_code = transpile(source);

    EXPECT_NE(transpiled_code.find(expected_read), std::string::npos);
    EXPECT_NE(transpiled_code.find(expected_write), std::string::npos);
}

TEST_F(TranspilerTest, FilesystemWithoutImport) {
    std::string source = R"(
        let content = fs_read("test.txt");
        fs_write("test.txt", "hello");
    )";
    std::string expected_read_error = "/* ERROR: Filesystem function 'fs_read' called without importing 'filesystem' */";
    std::string expected_write_error = "/* ERROR: Filesystem function 'fs_write' called without importing 'filesystem' */";

    std::string transpiled_code = transpile(source);

    EXPECT_NE(transpiled_code.find(expected_read_error), std::string::npos);
    EXPECT_NE(transpiled_code.find(expected_write_error), std::string::npos);
}

TEST_F(TranspilerTest, MetaIsInt) {
    std::string source = R"(
        let x: int = 10;
        let y: string = "hello";
        let is_x_int = meta::is_int(x);
        let is_y_int = meta::is_int(y);
    )";
    std::string expected = "const int x = 10;\nconst std::string y = \"hello\";\nconst bool is_x_int = true;\nconst bool is_y_int = false;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, OperatorOverloading) {
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

TEST_F(TranspilerTest, InputStatement) {
    std::string source = "import iostream; let name = input();";
    std::string expected = "const std::string name = input();\n";
    EXPECT_NE(transpile(source).find(expected), std::string::npos);
}

TEST_F(TranspilerTest, InputWithoutImport) {
    std::string source = "let name = input();";
    std::string expected_error = "/* ERROR: 'input' function called without importing 'iostream' */";
    std::string transpiled_code = transpile(source);
    EXPECT_NE(transpiled_code.find(expected_error), std::string::npos);
}

TEST_F(TranspilerTest, TraitDeclaration) {
    std::string source = "trait MyTrait { func foo(); func bar(a: int) -> bool; }";
    std::string expected = "struct MyTrait {\n    virtual ~MyTrait() = default;\n    virtual void foo() = 0;\n    virtual bool bar(int a) = 0;\n};\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, FileImport) {
    // Create the module file
    std::string module_content = "func add(a: int, b: int) -> int { return a + b; }";
    std::ofstream module_file("test_module.cns");
    module_file << module_content;
    module_file.close();

    // Get absolute path to the module
    std::filesystem::path module_path = std::filesystem::absolute("test_module.cns");
    std::string module_path_str = module_path.string();
    // On Windows, std::filesystem::path::string() uses backslashes, which are escape characters in C++.
    // We need to replace them with forward slashes for cross-platform compatibility in the import statement.
    std::replace(module_path_str.begin(), module_path_str.end(), '\\', '/');

    std::string source = "import \"" + module_path_str + "\"; let x = add(1, 2);";
    std::string expected = "int add(int a, int b) {\nreturn a + b;\n}\nconst auto x = add(1, 2);\n";
    EXPECT_EQ(transpile(source), expected);

    // Clean up
    std::filesystem::remove("test_module.cns");
}

TEST_F(TranspilerTest, StructAccessModifiers) {
    std::string source = R"(
        struct Test {
            public:
                x: int;
                func foo() {}
            private:
                y: int;
                func bar() {}
        }
    )";
    std::string expected = "struct Test {\npublic:\nint x;\nvoid foo() {\n}\nprivate:\nint y;\nvoid bar() {\n}\n};\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, InternalTraitImplementation) {
    std::string source = R"(
        trait MyTrait { func foo(); }
        struct MyStruct {
            public:
            impl MyTrait func foo() {}
        }
    )";
    std::string expected = "struct MyTrait {\n    virtual ~MyTrait() = default;\n    virtual void foo() = 0;\n};\nstruct MyStruct : public MyTrait {\npublic:\nvoid foo() override {\n}\n};\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, LambdaExpression) {
    std::string source = "let add = [](a: int, b: int) -> int { return a + b; };";
    std::string expected = "const auto add = [](int a, int b) -> int {\nreturn a + b;\n}\n;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, LambdaExpressionNoParams) {
	std::string source = "let get_five = []() -> int { return 5; };";
	std::string expected = "const auto get_five = []() -> int {\nreturn 5;\n}\n;\n";
	EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, LambdaExpressionWithCapture) {
	std::string source = "let x = 10; let add_x = [x](a: int) -> int { return a + x; };";
	std::string expected = "const int x = 10;\nconst auto add_x = [x](int a) -> int {\nreturn a + x;\n}\n;\n";
	EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, LambdaExpressionWithMultipleCaptures) {
	std::string source = "let x = 10; let y = 20; let add_xy = [x, y](a: int) -> int { return a + x + y; };";
	std::string expected = "const int x = 10;\nconst int y = 20;\nconst auto add_xy = [x, y](int a) -> int {\nreturn a + x + y;\n}\n;\n";
	EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, FunctionType) {
	std::string source = "let my_func: function(int, int) -> int = add;";
	std::string expected = "#include <functional>\nconst std::function<int(int, int)> my_func = add;\n";
	EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, LambdaExpressionWithReferenceCapture) {
	std::string source = "mut x = 10; let add_x = [&x](a: int) -> int { return a + x; };";
	std::string expected = "int x = 10;\nconst auto add_x = [&x](int a) -> int {\nreturn a + x;\n}\n;\n";
	EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, CustomBinaryOperator) {
    std::string source = R"(
        import operator;
        struct MyNumber impl operator::binary {
            public: val: int;
            public: func binary(&self, op: string, other: MyNumber) -> MyNumber {
                return MyNumber{val: self.val + other.val};
            }
        }
        func main() {
            let a = MyNumber{val: 10};
            let b = MyNumber{val: 3};
            let c: MyNumber = a ** b;
        }
    )";
    std::string expected_transpilation = "const MyNumber c = a.binary(\"**\", b);\n";
    std::string transpiled_code = transpile(source);
    // Check if the expected line exists in the transpiled code
    EXPECT_NE(transpiled_code.find(expected_transpilation), std::string::npos);
}

TEST_F(TranspilerTest, EnforceExplicitSelf) {
    std::string source = R"(
        struct Test {
            public:
                x: int;
                func bar() {}
                func foo() {
                    x = 1;
                    bar();
                }
        }
    )";
    std::string expected_field_error = "/* ERROR: Member 'x' cannot be accessed without 'self.'. */";
    std::string expected_method_error = "/* ERROR: Method 'bar' cannot be called without 'self.'. */";

    std::string transpiled_code = transpile(source);

    EXPECT_NE(transpiled_code.find(expected_field_error), std::string::npos);
    EXPECT_NE(transpiled_code.find(expected_method_error), std::string::npos);
}

TEST_F(TranspilerTest, BorrowImmutable) {
    std::string source = R"(
        let a = 10;
        let b = &a;
    )";
    std::string expected = "const int a = 10;\nconst int& b = a;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, BorrowMutableAndModify) {
    std::string source = R"(
        mut a = 10;
        mut b = &mut a;
        *b = 20;
    )";
    std::string expected = "int a = 10;\nint& b = a;\n*b = 20;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST_F(TranspilerTest, ReferenceAsFunctionParameter) {
    std::string source = R"(
        func increment(x: &mut int) {
            *x = *x + 1;
        }
        func main() {
            mut val = 10;
            increment(&mut val);
        }
    )";
    std::string expected = "void increment(int& x) {\n*x = *x + 1;\n}\nvoid main() {\nint val = 10;\nincrement(val);\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

