#include <gtest/gtest.h>
#include "Resolver.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <string>

using namespace chtholly;

class ResolverTest : public ::testing::Test {
protected:
    bool resolve(const std::string& source) {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();
        Parser parser(tokens);
        auto statements = parser.parse();
        Resolver resolver;
        resolver.resolve(statements);
        return resolver.hadError;
    }
};

TEST_F(ResolverTest, ValidBlock) {
    std::string source = "{ let a = 1; let b = 2; }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, RedeclareVariableInSameScope) {
    std::string source = "{ let a = 1; let a = 2; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ReadVariableInOwnInitializer) {
    std::string source = "{ let a = a; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, UseUndefinedVariable) {
    std::string source = "{ print(a); }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, AssignToUndefinedVariable) {
    std::string source = "{ a = 1; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, Shadowing) {
    std::string source = "{ let a = 1; { let a = 2; print(a); } print(a); }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ValidFunction) {
    std::string source = "func foo() { return 1; }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ReturnAtTopLevel) {
    std::string source = "return 1;";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidSelf) {
    std::string source = "struct Foo { func bar() { print(self); } }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, SelfOutsideOfClass) {
    std::string source = "print(self);";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, BinaryTypeError) {
    std::string source = R"(
        struct Foo {}
        let a = Foo{};
        let b = a + a;
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidTypeAnnotation) {
    std::string source = "let x: int = 10;";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, MismatchedTypeAnnotation) {
    std::string source = "let x: string = 10;";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, StructTypeAnnotation) {
    std::string source = R"(
        struct Foo {}
        let f: Foo = Foo{};
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ValidUnary) {
    std::string source = "let x = !true; let y = -10;";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, InvalidUnary) {
    std::string source = "let x = !123; let y = -\"hello\";";
    ASSERT_TRUE(resolve(source));
}
