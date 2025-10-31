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

TEST(Parser, TestGenericStructStatement) {
    std::string input = "struct Point<T> { x: T, y: T }";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Chtholly::StructStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    ASSERT_EQ(stmt->name->value, "Point");
    ASSERT_EQ(stmt->templateParams.size(), 1);
    ASSERT_EQ(stmt->templateParams[0]->value, "T");
    ASSERT_EQ(stmt->members.size(), 2);

    auto field1 = dynamic_cast<Chtholly::Field*>(stmt->members[0].get());
    ASSERT_NE(field1, nullptr);
    ASSERT_EQ(field1->name->value, "x");
    auto type1 = dynamic_cast<Chtholly::TypeName*>(field1->type.get());
    ASSERT_NE(type1, nullptr);
    ASSERT_EQ(type1->name, "T");

    auto field2 = dynamic_cast<Chtholly::Field*>(stmt->members[1].get());
    ASSERT_NE(field2, nullptr);
    ASSERT_EQ(field2->name->value, "y");
    auto type2 = dynamic_cast<Chtholly::TypeName*>(field2->type.get());
    ASSERT_NE(type2, nullptr);
    ASSERT_EQ(type2->name, "T");
}

TEST(Parser, TestGenericStructInstantiation) {
    std::string input = "let p: Point<int> = Point<int>{x: 0, y: 0};";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Chtholly::LetStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    ASSERT_EQ(stmt->name->value, "p");

    auto type = dynamic_cast<Chtholly::TypeName*>(stmt->type.get());
    ASSERT_NE(type, nullptr);
    ASSERT_EQ(type->name, "Point");
    ASSERT_EQ(type->templateArgs.size(), 1);
    auto typeArg = dynamic_cast<Chtholly::TypeName*>(type->templateArgs[0].get());
    ASSERT_NE(typeArg, nullptr);
    ASSERT_EQ(typeArg->name, "int");

    auto value = dynamic_cast<Chtholly::StructLiteral*>(stmt->value.get());
    ASSERT_NE(value, nullptr);

    auto structName = dynamic_cast<Chtholly::GenericInstantiation*>(value->structName.get());
    ASSERT_NE(structName, nullptr);

    auto base = dynamic_cast<Chtholly::Identifier*>(structName->base.get());
    ASSERT_NE(base, nullptr);
    ASSERT_EQ(base->value, "Point");
    ASSERT_EQ(structName->arguments.size(), 1);

    auto arg = dynamic_cast<Chtholly::TypeName*>(structName->arguments[0].get());
    ASSERT_NE(arg, nullptr);
    ASSERT_EQ(arg->name, "int");

    ASSERT_EQ(value->fields.size(), 2);
    ASSERT_EQ(value->fields[0].first->value, "x");
    auto val1 = dynamic_cast<Chtholly::IntegerLiteral*>(value->fields[0].second.get());
    ASSERT_NE(val1, nullptr);
    ASSERT_EQ(val1->value, 0);

    ASSERT_EQ(value->fields[1].first->value, "y");
    auto val2 = dynamic_cast<Chtholly::IntegerLiteral*>(value->fields[1].second.get());
    ASSERT_NE(val2, nullptr);
    ASSERT_EQ(val2->value, 0);
}

TEST(Parser, TestMemberAccessExpression) {
    std::string input = "p.x;";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Chtholly::ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto expr = dynamic_cast<Chtholly::MemberAccessExpression*>(stmt->expression.get());
    ASSERT_NE(expr, nullptr);

    auto obj = dynamic_cast<Chtholly::Identifier*>(expr->object.get());
    ASSERT_NE(obj, nullptr);
    ASSERT_EQ(obj->value, "p");

    ASSERT_EQ(expr->member->value, "x");
}

TEST(Parser, TestEnumStatement) {
    std::string input = "enum Color { Red, Green, Blue }";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Chtholly::EnumStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    ASSERT_EQ(stmt->name->value, "Color");
    ASSERT_EQ(stmt->members.size(), 3);
    ASSERT_EQ(stmt->members[0]->value, "Red");
    ASSERT_EQ(stmt->members[1]->value, "Green");
    ASSERT_EQ(stmt->members[2]->value, "Blue");
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
