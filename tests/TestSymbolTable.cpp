#include "gtest/gtest.h"
#include "SymbolTable.h"
#include "Type.h"

using namespace chtholly;

class SymbolTableTest : public ::testing::Test {
protected:
    SymbolTable table;
    std::shared_ptr<Type> int_type = std::make_shared<BasicType>("int");
    std::shared_ptr<Type> string_type = std::make_shared<BasicType>("string");
};

TEST_F(SymbolTableTest, DefineAndResolveInGlobalScope) {
    table.define("a", int_type);
    Token token_a(TokenType::IDENTIFIER, "a", nullptr, 1);
    EXPECT_EQ(table.resolve(token_a), int_type);

    Token token_b(TokenType::IDENTIFIER, "b", nullptr, 1);
    EXPECT_EQ(table.resolve(token_b), nullptr);
}

TEST_F(SymbolTableTest, NestedScopes) {
    table.define("a", int_type); // Global scope
    table.enter_scope();
    table.define("b", string_type); // Inner scope

    Token token_a(TokenType::IDENTIFIER, "a", nullptr, 1);
    EXPECT_EQ(table.resolve(token_a), int_type);

    Token token_b(TokenType::IDENTIFIER, "b", nullptr, 1);
    EXPECT_EQ(table.resolve(token_b), string_type);

    table.exit_scope();

    EXPECT_EQ(table.resolve(token_a), int_type);
    EXPECT_EQ(table.resolve(token_b), nullptr); // b is out of scope
}

TEST_F(SymbolTableTest, Shadowing) {
    table.define("a", int_type);
    table.enter_scope();
    table.define("a", string_type); // Shadowing a

    Token token_a(TokenType::IDENTIFIER, "a", nullptr, 1);
    EXPECT_EQ(table.resolve(token_a), string_type);

    table.exit_scope();
    EXPECT_EQ(table.resolve(token_a), int_type);
}

TEST_F(SymbolTableTest, IsDefinedInCurrentScope) {
    table.define("a", int_type);
    EXPECT_TRUE(table.is_defined_in_current_scope("a"));
    EXPECT_FALSE(table.is_defined_in_current_scope("b"));

    table.enter_scope();
    table.define("b", string_type);
    EXPECT_FALSE(table.is_defined_in_current_scope("a"));
    EXPECT_TRUE(table.is_defined_in_current_scope("b"));
}
