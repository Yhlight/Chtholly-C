#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"
#include "Resolver.h"
#include "Transpiler.h"
#include "Error.h"

class TraitTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(TraitTest, TraitDeclaration) {
    std::string source = "trait MyTrait { my_func(a: int) -> void; }";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    ASSERT_NE(dynamic_cast<TraitStmt*>(stmts[0].get()), nullptr);
}

TEST_F(TraitTest, ImplDeclaration) {
    std::string source = "struct MyStruct {} impl MyTrait for MyStruct { my_func(a: int) -> void {} }";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 2);
    ASSERT_NE(dynamic_cast<StructStmt*>(stmts[0].get()), nullptr);
    ASSERT_NE(dynamic_cast<ImplStmt*>(stmts[1].get()), nullptr);
}

TEST_F(TraitTest, TranspileTrait) {
    std::string source = "trait MyTrait { my_func(a: int) -> void; }";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string output = transpiler.transpile(stmts);
    std::string expected = "struct MyTrait {\n    virtual void my_func(int a) = 0;\n};\n\n";
    ASSERT_TRUE(output.find(expected) != std::string::npos);
}

TEST_F(TraitTest, TranspileImpl) {
    std::string source = "trait MyTrait { my_func(a: int) -> void; } struct MyStruct {} impl MyTrait for MyStruct { my_func(a: int) -> void {} }";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string output = transpiler.transpile(stmts);
    std::string expected = "struct MyStruct : public MyTrait {\n";
    ASSERT_TRUE(output.find(expected) != std::string::npos);
}

TEST_F(TraitTest, ResolveImpl) {
    std::string source = "trait MyTrait { my_func(a: int) -> void; } struct MyStruct {} impl MyTrait for MyStruct { my_func(a: int) -> void {} }";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(TraitTest, ResolveImplMissingMethod) {
    std::string source = "trait MyTrait { my_func(a: int) -> void; } struct MyStruct {} impl MyTrait for MyStruct {}";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(TraitTest, ResolveImplWrongSignature) {
    std::string source = "trait MyTrait { my_func(a: int) -> void; } struct MyStruct {} impl MyTrait for MyStruct { my_func(a: string) -> void {} }";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(TraitTest, ResolveImplNonExistentTrait) {
    std::string source = "struct MyStruct {} impl MyTrait for MyStruct {}";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}
