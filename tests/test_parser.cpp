#include <gtest/gtest.h>
#include "Parser.h"
#include "AST.h"
#include <vector>
#include <variant>
#include <string>

// Helper function to stringify the AST for easy comparison
std::string astToString(Chtholly::Node* node) {
    if (!node) {
        return "";
    }
    if (auto p = dynamic_cast<Chtholly::Program*>(node)) {
        std::string s;
        for (const auto& stmt : p->statements) {
            s += astToString(stmt.get());
        }
        return s;
    }
    if (auto es = dynamic_cast<Chtholly::ExpressionStatement*>(node)) {
        return astToString(es->expression.get());
    }
    if (auto il = dynamic_cast<Chtholly::IntegerLiteral*>(node)) {
        return std::to_string(il->value);
    }
    if (auto pe = dynamic_cast<Chtholly::PrefixExpression*>(node)) {
        return "(" + pe->op + astToString(pe->right.get()) + ")";
    }
    if (auto ie = dynamic_cast<Chtholly::InfixExpression*>(node)) {
        return "(" + astToString(ie->left.get()) + " " + ie->op + " " + astToString(ie->right.get()) + ")";
    }
    if (auto b = dynamic_cast<Chtholly::Boolean*>(node)) {
        return b->value ? "true" : "false";
    }
    if (auto i = dynamic_cast<Chtholly::Identifier*>(node)) {
        return i->value;
    }
    return "";
}


void checkParserErrors(Chtholly::Parser& p) {
    const auto& errors = p.errors();
    if (errors.empty()) {
        return;
    }

    std::cerr << "Parser has " << errors.size() << " errors" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "parser error: " << msg << std::endl;
    }
    FAIL();
}

TEST(Parser, TestOperatorPrecedence) {
    std::vector<std::pair<std::string, std::string>> tests = {
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
        {"a + b + c", "((a + b) + c)"},
        {"a + b - c", "((a + b) - c)"},
        {"a * b * c", "((a * b) * c)"},
        {"a * b / c", "((a * b) / c)"},
        {"a + b / c", "(a + (b / c))"},
        {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
        {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
        {"true", "true"},
        {"false", "false"},
        {"3 > 5 == false", "((3 > 5) == false)"},
        {"3 < 5 == true", "((3 < 5) == true)"},
    };

    for (const auto& test : tests) {
        Chtholly::Lexer l(test.first);
        Chtholly::Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);

        auto str = astToString(program.get());
        ASSERT_EQ(str, test.second);
    }
}
