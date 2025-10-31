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

TEST(CodeGen, TestImportAndPrint) {
    std::string input = R"(
        import iostream;
        print("Hello, World!");
    )";
    std::string expected = "#include <iostream>\n\ntemplate<typename T>\nvoid print(T value) {\n    std::cout << value << std::endl;\n}\n\nprint(\"Hello, World!\");\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestGenericFunctionWithConstraints) {
    std::string input = "func my_func<T ? MyTrait>(arg: T) {}";
    std::string expected = "const auto my_func = template<typename T>\nstatic_assert(std::is_base_of<MyTrait, T>::value, \"Type does not implement trait\");\n[&](T arg) {\n}\n;\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestTraitAndImplStatement) {
    std::string input = R"(
        trait MyTrait {}
        struct MyStruct {}
        impl MyStruct MyTrait {}
    )";
    std::string expected = "struct MyStruct : public MyTrait {\n};\n";

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
