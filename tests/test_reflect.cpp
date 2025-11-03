#include <gtest/gtest.h>
#include "Chtholly.h"
#include "Error.h"
#include "AST/ASTPrinter.h"

// Test fixture for reflection tests
class ReflectTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(ReflectTest, ParseGetFieldNames) {
    std::string source = R"(
        struct MyStruct {
            let a: int;
            let b: string;
        }
        reflect.get_field_names<MyStruct>();
    )";
    Chtholly chtholly;
    auto statements = chtholly.run(source, true);
    ASSERT_FALSE(ErrorReporter::hadError);
    ASSERT_EQ(statements.size(), 2);

    auto* exprStmt = dynamic_cast<ExpressionStmt*>(statements[1].get());
    ASSERT_NE(exprStmt, nullptr);

    auto* callExpr = dynamic_cast<CallExpr*>(exprStmt->expression.get());
    ASSERT_NE(callExpr, nullptr);

    auto* getExpr = dynamic_cast<GetExpr*>(callExpr->callee.get());
    ASSERT_NE(getExpr, nullptr);

    auto* varExpr = dynamic_cast<VariableExpr*>(getExpr->object.get());
    ASSERT_NE(varExpr, nullptr);
    ASSERT_EQ(varExpr->name.lexeme, "reflect");

    ASSERT_EQ(getExpr->name.lexeme, "get_field_names");

    ASSERT_EQ(callExpr->generic_args.size(), 1);
    ASSERT_EQ(callExpr->generic_args[0].baseType.lexeme, "MyStruct");
}

TEST_F(ReflectTest, ResolveValidGetFieldNames) {
    std::string source = R"(
        struct MyStruct {
            let a: int;
        }
        let names = reflect.get_field_names<MyStruct>();
    )";
    Chtholly chtholly;
    auto statements = chtholly.run(source, true);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(ReflectTest, ResolveGetFieldNamesWithNonStruct) {
    std::string source = R"(
        let names = reflect.get_field_names<int>();
    )";
    Chtholly chtholly;
    chtholly.run(source, true);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ReflectTest, TranspileGetFieldNames) {
    std::string source = R"(
        struct MyStruct {
            let a: int;
            let b: string;
        }
        let names = reflect.get_field_names<MyStruct>();
    )";
    Chtholly chtholly;
    std::string output = chtholly.run(source);
    std::string expected = R"(#include <iostream>
#include <variant>
#include <string>
#include <vector>

std::string chtholly_input() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

struct MyStruct {
public:
    int a;
    std::string b;
};

int main() {
    const auto names = std::vector<std::string>{"a", "b"};
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}
