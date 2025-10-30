#include <gtest/gtest.h>
#include "Parser.h"
#include "AST.h"
#include <vector>
#include <variant>
#include <string>

// Helper function to stringify the AST for easy comparison
std::string astToString(Chtholly::Node* node);

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

TEST(Parser, TestFunctionLiteralParsing) {
    std::string input = "func(x, y) { x + y; }";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Chtholly::ExpressionStatement*>(program->statements[0].get());
    auto exp = dynamic_cast<Chtholly::FunctionLiteral*>(stmt->expression.get());
    ASSERT_NE(exp, nullptr);
    ASSERT_EQ(exp->parameters.size(), 2);
    ASSERT_EQ(exp->parameters[0]->value, "x");
    ASSERT_EQ(exp->parameters[1]->value, "y");
    ASSERT_EQ(exp->body->statements.size(), 1);
    auto bodyStmt = dynamic_cast<Chtholly::ExpressionStatement*>(exp->body->statements[0].get());
    ASSERT_NE(bodyStmt, nullptr);
    ASSERT_EQ(astToString(bodyStmt), "(x + y)");
}

TEST(Parser, TestCallExpressionParsing) {
    std::string input = "add(1, 2 * 3, 4 + 5);";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Chtholly::ExpressionStatement*>(program->statements[0].get());
    auto exp = dynamic_cast<Chtholly::CallExpression*>(stmt->expression.get());
    ASSERT_NE(exp, nullptr);
    ASSERT_EQ(astToString(exp->function.get()), "add");
    ASSERT_EQ(exp->arguments.size(), 3);
    ASSERT_EQ(astToString(exp->arguments[0].get()), "1");
    ASSERT_EQ(astToString(exp->arguments[1].get()), "(2 * 3)");
    ASSERT_EQ(astToString(exp->arguments[2].get()), "(4 + 5)");
}

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
    if (auto ife = dynamic_cast<Chtholly::IfExpression*>(node)) {
        std::string s = "if" + astToString(ife->condition.get()) + " " + astToString(ife->consequence.get());
        if (ife->alternative) {
            s += "else " + astToString(ife->alternative.get());
        }
        return s;
    }
    if (auto bs = dynamic_cast<Chtholly::BlockStatement*>(node)) {
        std::string s;
        for (const auto& stmt : bs->statements) {
            s += astToString(stmt.get());
        }
        return s;
    }
    return "";
}
