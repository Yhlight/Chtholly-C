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
