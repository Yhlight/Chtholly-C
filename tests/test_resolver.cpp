#include <gtest/gtest.h>
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Resolver.h"

TEST(ResolverTest, Redeclaration) {
    std::string source = "func main() { let x = 10; let x = 20; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_THROW(resolver.resolve(statements), std::runtime_error);
}

TEST(ResolverTest, ValidStruct) {
    std::string source = "struct MyStruct { public name: string; }; func main() { let s = MyStruct { name: \"hello\" }; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, InvalidStructInitializer) {
    std::string source_wrong_field = "struct S { public f: int; }; func main() { let s = S { g: 1 }; }";
    std::string source_wrong_count = "struct S { public f: int; }; func main() { let s = S { f: 1, g: 2 }; }";
    std::string source_wrong_type = "struct S { public f: int; }; func main() { let s = S { f: \"hello\" }; }";

    Lexer lexer1(source_wrong_field);
    Parser parser1(lexer1);
    auto statements1 = parser1.parse();
    Resolver resolver1;
    EXPECT_THROW(resolver1.resolve(statements1), std::runtime_error);

    Lexer lexer2(source_wrong_count);
    Parser parser2(lexer2);
    auto statements2 = parser2.parse();
    Resolver resolver2;
    EXPECT_THROW(resolver2.resolve(statements2), std::runtime_error);

    Lexer lexer3(source_wrong_type);
    Parser parser3(lexer3);
    auto statements3 = parser3.parse();
    Resolver resolver3;
    EXPECT_THROW(resolver3.resolve(statements3), std::runtime_error);
}

TEST(ResolverTest, PrivateFieldAccess) {
    std::string source = "struct S { private f: int; }; func main() { let s = S { f: 1 }; let f = s.f; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_THROW(resolver.resolve(statements), std::runtime_error);
}

TEST(ResolverTest, ValidMutableBorrow) {
    std::string source = "func main() { mut x = 10; let y = &mut x; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, InvalidMutableBorrow) {
    std::string source = "func main() { let x = 10; let y = &mut x; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_THROW(resolver.resolve(statements), std::runtime_error);
}

TEST(ResolverTest, UseAfterMove) {
    std::string source = "func main() { let x = 10; let y = x; let z = x; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_THROW(resolver.resolve(statements), std::runtime_error);
}

TEST(ResolverTest, BooleanExpressions) {
    std::string source = "func main() { let x = true; let y = false; let z = x == y; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, UnaryNot) {
    std::string source = "func main() { let x = !true; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, InvalidUnaryNot) {
    std::string source = "func main() { let x = !10; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_THROW(resolver.resolve(statements), std::runtime_error);
}

TEST(ResolverTest, ComparisonOperators) {
    std::string source = "func main() { let x = 10 > 5; let y = 10 < 5; let z = 10 >= 10; let w = 10 <= 10; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, Scope) {
    std::string source = "func main() { let x = 10; { let x = 20; } let y = x; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, TypeMismatch) {
    std::string source = "func main() { let x = 10 + \"hello\"; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_THROW(resolver.resolve(statements), std::runtime_error);
}
