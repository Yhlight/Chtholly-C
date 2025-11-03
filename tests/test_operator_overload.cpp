#include <gtest/gtest.h>
#include "../src/Transpiler.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Resolver.h"
#include "../src/Error.h"

TEST(OperatorOverloadTest, PointAddition) {
    std::string source = R"(
        trait Add<T> {
            operator::add(other: T) -> T;
        }

        struct Point {
            let x: double;
            let y: double;
        }

        impl Add<Point> for Point {
            operator::add(other: Point) -> Point {
                return Point{x: self.x + other.x, y: self.y + other.y};
            }
        }

        let p1 = Point{x: 1.0, y: 2.0};
        let p2 = Point{x: 3.0, y: 4.0};
        let p3 = p1 + p2;
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_FALSE(ErrorReporter::hadError);

    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected = "p1.add(p2)";
    ASSERT_TRUE(result.find(expected) != std::string::npos);
}
