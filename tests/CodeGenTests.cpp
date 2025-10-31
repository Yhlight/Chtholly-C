#include <gtest/gtest.h>
#include "CodeGen.h"
#include "Parser.h"
#include "Lexer.h"

using namespace chtholly;

TEST(CodeGenTest, SimpleVariableDeclaration) {
    std::string source = "let a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "const auto a = 10;\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, MutableVariableDeclaration) {
    std::string source = "mut b = 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "auto b = 20;\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, SimpleBinaryExpression) {
    std::string source = "let c = 10 + 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "const auto c = (10 + 20);\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, FunctionDeclaration) {
    std::string source = "func add(x: int, y: int) -> int { let z = x + y; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "int add(int x, int y) {\nconst auto z = (x + y);\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, FunctionCall) {
    std::string source = "func add(x: int, y: int) -> int { let z = x + y; }\nlet a = add(1, 2);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "int add(int x, int y) {\nconst auto z = (x + y);\n}\nconst auto a = add(1, 2);\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, IfStatement) {
    std::string source = "if (true) { let a = 1; } else { let b = 2; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "if (true) {\nconst auto a = 1;\n}\nelse {\nconst auto b = 2;\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, SwitchStatement) {
    std::string source = "let x = 1; switch (x) { case 1: { let a = 1; } case 2: { let b = 2; } }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "const auto x = 1;\nswitch (x) {\ncase 1: {\nconst auto a = 1;\n}\ncase 2: {\nconst auto b = 2;\n}\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, SwitchStatementWithDefault) {
    std::string source = "let x = 1; switch (x) { case 1: { let a = 1; } default: { let b = 2; } }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "const auto x = 1;\nswitch (x) {\ncase 1: {\nconst auto a = 1;\n}\ndefault: {\nconst auto b = 2;\n}\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, EnumDeclaration) {
    std::string source = "enum Color { Red, Green, Blue };";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "enum class Color {\n    Red,\n    Green,\n    Blue\n};\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, WhileStatement) {
    std::string source = "while (true) { let a = 1; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "while (true) {\nconst auto a = 1;\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, ForStatement) {
    std::string source = "for (mut i = 0; true; i = i + 1) { let a = 1; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "for (auto i = 0;true;i = (i + 1)) {\nconst auto a = 1;\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, ImportStatement) {
    std::string source = "import iostream;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "#include <iostream>\n#include <string>\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, PrintStatement) {
    std::string source = "import iostream; print(\"Hello, World!\");";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "#include <iostream>\n#include <string>\nstd::cout << \"Hello, World!\" << std::endl;\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, StructDeclaration) {
    std::string source = "struct Point { public x: int, private y: int }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "struct Point {\npublic: int x;\nprivate: int y;\n};\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, StructInstantiationAndMemberAccess) {
    std::string source = "struct Point { public x: int, public y: int }\n"
                         "let p = Point{x: 10, y: 20};\n"
                         "let x = p.x;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "struct Point {\npublic: int x;\npublic: int y;\n};\n"
                         "const auto p = Point{.x = 10, .y = 20};\n"
                         "const auto x = p.x;\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, TraitDeclaration) {
    std::string source = "trait MyTrait { func my_method(x: int) -> void; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Sema sema;
    auto ast = parser.parse();
    sema.analyze(*ast);
    CodeGen codeGen(sema);
    std::string expected = "class MyTrait {\npublic:\n    virtual ~MyTrait() = default;\n    virtual void my_method(int x) = 0;\n};\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}
