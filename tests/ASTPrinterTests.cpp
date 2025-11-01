#include <gtest/gtest.h>
#include "AST/ASTPrinter.h"
#include "AST/Expr.h"
#include "Token.h"
#include <memory>

TEST(ASTPrinterTest, PrintSimpleExpression) {
    // Represents the expression: -123 * (45.67)
    auto expr = std::make_unique<BinaryExpr>(
        std::make_unique<UnaryExpr>(
            Token{TokenType::MINUS, "-", std::monostate{}, 1},
            std::make_unique<LiteralExpr>(123.0)
        ),
        Token{TokenType::STAR, "*", std::monostate{}, 1},
        std::make_unique<GroupingExpr>(
            std::make_unique<LiteralExpr>(45.67)
        )
    );

    ASTPrinter printer;
    std::string result = printer.print(*expr);
    EXPECT_EQ(result, "(* (- 123.000000) (group 45.670000))");
}
