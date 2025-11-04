#include <gtest/gtest.h>
#include "../src/backend/Transpiler.h"
#include "../src/frontend/Lexer.h"
#include "../src/frontend/Parser.h"
#include "../src/middle/Resolver.h"
#include "../src/common/Error.h"

TEST(SimpleTraitTest, PointPrint) {
    std::string source = R"(
        trait Printable {
            print();
        }

        struct Point {
            let x: double;
            let y: double;
        }

        impl Printable for Point {
            print() {
                print(self.x);
                print(self.y);
            }
        }

        let p = Point{x: 1.0, y: 2.0};
        p.print();
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
    std::string expected = "p.print()";
    ASSERT_TRUE(result.find(expected) != std::string::npos);
}
