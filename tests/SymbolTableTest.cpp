#include <gtest/gtest.h>
#include "SymbolTable.h"
#include "Type.h"

using namespace chtholly;

TEST(SymbolTableTest, BasicDefineAndLookup) {
    SymbolTable st;
    st.define("x", std::make_unique<IntType>(), Mutability::Immutable);
    Symbol* symbol = st.lookup("x");
    ASSERT_NE(symbol, nullptr);
    EXPECT_EQ(symbol->name, "x");
    EXPECT_EQ(dynamic_cast<IntType*>(symbol->type.get()) != nullptr, true);
    EXPECT_EQ(symbol->mutability, Mutability::Immutable);

    EXPECT_EQ(st.lookup("y"), nullptr);
}

TEST(SymbolTableTest, Redefinition) {
    SymbolTable st;
    EXPECT_TRUE(st.define("x", std::make_unique<IntType>(), Mutability::Immutable));
    EXPECT_FALSE(st.define("x", std::make_unique<StringType>(), Mutability::Mutable)); // Redefinition in same scope
}

TEST(SymbolTableTest, NestedScopeLookup) {
    SymbolTable st;
    st.define("x", std::make_unique<IntType>(), Mutability::Immutable);
    st.enterScope();
    st.define("y", std::make_unique<StringType>(), Mutability::Mutable);

    Symbol* x_symbol = st.lookup("x");
    ASSERT_NE(x_symbol, nullptr);
    EXPECT_EQ(x_symbol->name, "x");

    Symbol* y_symbol = st.lookup("y");
    ASSERT_NE(y_symbol, nullptr);
    EXPECT_EQ(y_symbol->name, "y");
}

TEST(SymbolTableTest, ScopeShadowing) {
    SymbolTable st;
    st.define("x", std::make_unique<IntType>(), Mutability::Immutable);
    st.enterScope();
    st.define("x", std::make_unique<StringType>(), Mutability::Mutable);

    Symbol* symbol = st.lookup("x");
    ASSERT_NE(symbol, nullptr);
    EXPECT_EQ(symbol->name, "x");
    EXPECT_EQ(dynamic_cast<StringType*>(symbol->type.get()) != nullptr, true); // Should find the inner scope's StringType
    EXPECT_EQ(symbol->mutability, Mutability::Mutable);
}

TEST(SymbolTableTest, ScopeExit) {
    SymbolTable st;
    st.define("x", std::make_unique<IntType>(), Mutability::Immutable);
    st.enterScope();
    st.define("y", std::make_unique<StringType>(), Mutability::Mutable);

    st.exitScope();

    EXPECT_NE(st.lookup("x"), nullptr); // x should still be visible
    EXPECT_EQ(st.lookup("y"), nullptr); // y should not be visible
}
