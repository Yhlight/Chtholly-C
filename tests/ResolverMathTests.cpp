#include <gtest/gtest.h>
#include "Resolver.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <string>

using namespace chtholly;

class ResolverMathTest : public ::testing::Test {
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

TEST_F(ResolverMathTest, ValidMathCalls) {
    std::string source = R"(
        import math;
        func main() {
            let a = math::sqrt(4.0);
            let b = math::pow(2.0, 3.0);
            let c = math::sin(0.5);
        }
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverMathTest, WrongNumberOfArguments) {
    std::string source_sqrt = R"(
        import math;
        func main() {
            let a = math::sqrt(4.0, 5.0);
        }
    )";
    ASSERT_TRUE(resolve(source_sqrt));

    std::string source_pow = R"(
        import math;
        func main() {
            let a = math::pow(2.0);
        }
    )";
    ASSERT_TRUE(resolve(source_pow));
}

TEST_F(ResolverMathTest, WrongArgumentType) {
    std::string source = R"(
        import math;
        func main() {
            let a = math::sqrt("not a number");
        }
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverMathTest, UnknownMathFunction) {
    std::string source = R"(
        import math;
        func main() {
            let a = math::non_existent_function(1.0);
        }
    )";
    ASSERT_TRUE(resolve(source));
}
