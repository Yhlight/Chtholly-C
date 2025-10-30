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

TEST(ResolverTest, Scope) {
    std::string source = "func main() { let x = 10; { let x = 20; } let y = x; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Resolver resolver;
    EXPECT_NO_THROW(resolver.resolve(statements));
}
