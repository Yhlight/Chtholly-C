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

TEST(Parser, TestStructStatement) {
    std::string input = R"(
        struct MyStruct {
            public name: string,
            private id: int,

            public add(x: int, y: int) -> int {
                return x + y;
            }
        }
    )";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    ASSERT_EQ(program->statements.size(), 1);
    auto stmt = dynamic_cast<Chtholly::StructStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    ASSERT_EQ(stmt->name->value, "MyStruct");
    ASSERT_EQ(stmt->members.size(), 3);

    auto field1 = dynamic_cast<Chtholly::Field*>(stmt->members[0].get());
    ASSERT_NE(field1, nullptr);
    ASSERT_EQ(field1->isPublic, true);
    ASSERT_EQ(field1->name->value, "name");
    auto type1 = dynamic_cast<Chtholly::TypeName*>(field1->type.get());
    ASSERT_NE(type1, nullptr);
    ASSERT_EQ(type1->name, "string");

    auto field2 = dynamic_cast<Chtholly::Field*>(stmt->members[1].get());
    ASSERT_NE(field2, nullptr);
    ASSERT_EQ(field2->isPublic, false);
    ASSERT_EQ(field2->name->value, "id");
    auto type2 = dynamic_cast<Chtholly::TypeName*>(field2->type.get());
    ASSERT_NE(type2, nullptr);
    ASSERT_EQ(type2->name, "int");

    auto method = dynamic_cast<Chtholly::Method*>(stmt->members[2].get());
    ASSERT_NE(method, nullptr);
    ASSERT_EQ(method->isPublic, true);
    ASSERT_EQ(method->name->value, "add");
    ASSERT_EQ(method->function->parameters.size(), 2);
    ASSERT_EQ(method->function->parameters[0]->value, "x");
    ASSERT_EQ(method->function->parameters[1]->value, "y");
    auto return_type = dynamic_cast<Chtholly::TypeName*>(method->function->returnType.get());
    ASSERT_NE(return_type, nullptr);
    ASSERT_EQ(return_type->name, "int");
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
