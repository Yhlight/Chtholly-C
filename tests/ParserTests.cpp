#include <gtest/gtest.h>
#include "Parser.h"
#include "AstPrinter.h"
#include "Lexer.h"
#include <vector>
#include <string>

using namespace chtholly;

std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    AstPrinter printer;
    return printer.print(statements);
}

TEST(ParserTest, BasicExpression) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "(; (+ 1 (* 2 3)))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ParenthesizedExpression) {
    std::string source = "(1 + 2) * 3;";
    std::string expected = "(; (* (group (+ 1 2)) 3))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, UnaryExpression) {
    std::string source = "-1 * -2;";
    std::string expected = "(; (* (- 1) (- 2)))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, VariableDeclaration) {
    std::string source = "let x = 10;";
    std::string expected = "(let x 10)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, MutableVariableDeclaration) {
    std::string source = "mut y = \"hello\";";
    std::string expected = "(mut y hello)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, AssignmentExpression) {
    std::string source = "x = 5;";
    std::string expected = "(; (= x 5))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, MultipleStatements) {
    std::string source = "let x = 1; let y = 2;";
    std::string expected = "(let x 1)(let y 2)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, IfStatement) {
    std::string source = "if (x > 1) 1;";
    std::string expected = "(if (> x 1) (; 1))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, IfElseStatement) {
    std::string source = "if (x > 1) 1; else 2;";
    std::string expected = "(if-else (> x 1) (; 1) (; 2))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, WhileStatement) {
    std::string source = "while (x > 0) x = x - 1;";
    std::string expected = "(while (> x 0) (; (= x (- x 1))))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ForStatement) {
    std::string source = "for (let i = 0; i < 10; i = i + 1) print(i);";
    std::string expected = "(for (let i 0) (< i 10) (= i (+ i 1)) (; (call print i)))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, BlockStatement) {
    std::string source = "{ let x = 1; let y = 2; }";
    std::string expected = "(block (let x 1) (let y 2))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, FunctionDeclaration) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    std::string expected = "(func add (block (return (+ a b))))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, StructDeclaration) {
    std::string source = "struct Point { let x: int; let y: int; }";
    std::string expected = "(struct Point (let x) (let y))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, StructWithTrait) {
    std::string source = "struct Point impl operator::add { let x: int; }";
    std::string expected = "(struct Point (impl (. operator add)) (let x))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ReturnStatement) {
    std::string source = "return 1;";
    std::string expected = "(return 1)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, CallExpression) {
    std::string source = "add(1, 2);";
    std::string expected = "(; (call add 1 2))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ComprehensiveTest) {
    std::string source = R"(
        func main() {
            let x = 10;
            if (x > 5) {
                x = x - 1;
            } else {
                x = 0;
            }
            while (x > 0) {
                x = x - 1;
            }
            return x;
        }
    )";
    std::string expected = "(func main (block (let x 10) (if-else (> x 5) (block (; (= x (- x 1)))) (block (; (= x 0)))) (while (> x 0) (block (; (= x (- x 1))))) (return x)))";
    EXPECT_EQ(parseAndPrint(source), expected);
}
