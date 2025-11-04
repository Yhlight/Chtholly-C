#include <gtest/gtest.h>
#include "AstPrinter.h"
#include "AST.h"
#include "Token.h"

using namespace chtholly;

TEST(AstPrinterTest, BasicPrint) {
    // -2 * (4 + 3)
    auto expr = std::make_unique<BinaryExpr>(
        std::make_unique<UnaryExpr>(
            Token(TokenType::MINUS, "-", nullptr, 1),
            std::make_unique<LiteralExpr>(2LL)
        ),
        Token(TokenType::STAR, "*", nullptr, 1),
        std::make_unique<GroupingExpr>(
            std::make_unique<BinaryExpr>(
                std::make_unique<LiteralExpr>(4LL),
                Token(TokenType::PLUS, "+", nullptr, 1),
                std::make_unique<LiteralExpr>(3LL)
            )
        )
    );

    AstPrinter printer;
    std::string result = std::any_cast<std::string>(expr->accept(printer));
    EXPECT_EQ(result, "(* (- 2) (group (+ 4 3)))");
}
