#include <gtest/gtest.h>
#include "Resolver.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <string>

using namespace chtholly;

class ResolverTest : public ::testing::Test {
protected:
    bool resolve(const std::string& source) {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();
        Parser parser(tokens);
        auto statements = parser.parse();
        Resolver resolver;
        resolver.resolve(statements);
        return resolver.hadError;
    }
};

TEST_F(ResolverTest, ValidBlock) {
    std::string source = "{ let a = 1; let b = 2; }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, RedeclareVariableInSameScope) {
    std::string source = "{ let a = 1; let a = 2; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ReadVariableInOwnInitializer) {
    std::string source = "{ let a = a; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, UseUndefinedVariable) {
    std::string source = "{ print(a); }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, AssignToUndefinedVariable) {
    std::string source = "{ a = 1; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, Shadowing) {
    std::string source = "{ let a = 1; { let a = 2; print(a); } print(a); }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ValidFunction) {
    std::string source = "func foo() -> int { return 1; }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ReturnAtTopLevel) {
    std::string source = "return 1;";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidSelf) {
    std::string source = "struct Foo { func bar() { print(self); } }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, SelfOutsideOfClass) {
    std::string source = "print(self);";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, BinaryTypeError) {
    std::string source = R"(
        struct Foo {}
        let a = Foo{};
        let b = a + a;
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidControlFlowCondition) {
    std::string source = R"(
        if (true) {}
        while (false) {}
        for (let i = 0; i < 10; i = i + 1) {}
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, InvalidIfCondition) {
    std::string source = "if (10) {}";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, InvalidWhileCondition) {
    std::string source = "while (10) {}";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, InvalidForCondition) {
    std::string source = "for (; 10; ) {}";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidReturnType) {
    std::string source = R"(
        func foo() -> int {
            return 1;
        }
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ReturnValueFromVoidFunction) {
    std::string source = R"(
        func foo() {
            return 1;
        }
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ReturnVoidFromNonVoidFunction) {
    std::string source = R"(
        func foo() -> int {
            return;
        }
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, MismatchedReturnType) {
    std::string source = R"(
        func foo() -> int {
            return "hello";
        }
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ComparisonExpressionHasBooleanType) {
    std::string source = R"(
        let b: bool = 1 < 2;
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ValidStructInitialization) {
    std::string source = R"(
        struct Point {
            x: int;
            y: int;
        }
        let p = Point{x: 1, y: 2};
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, MismatchedAssignmentType) {
    std::string source = R"(
        let x: int = 1;
        x = "hello";
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, StructInitializationWithUndefinedField) {
    std::string source = R"(
        struct Point {
            x: int;
            y: int;
        }
        let p = Point{x: 1, z: 2};
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, StructInitializationWithMissingField) {
    std::string source = R"(
        struct Point {
            x: int;
            y: int;
        }
        let p = Point{x: 1};
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, StructInitializationWithMismatchedType) {
    std::string source = R"(
        struct Point {
            x: int;
            y: int;
        }
        let p = Point{x: 1, y: "hello"};
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidFieldAccess) {
    std::string source = R"(
        struct Foo {
            bar: int;
        }
        let foo = Foo{bar: 1};
        foo.bar;
    )";
    ASSERT_FALSE(resolve(source));
    // Here we would ideally also check that the type of `foo.bar` is int.
    // This requires a more advanced test setup to inspect the AST after resolution.
}

TEST_F(ResolverTest, InvalidFieldAccess) {
    std::string source = R"(
        struct Foo {
            bar: int;
        }
        let foo = Foo{bar: 1};
        foo.baz;
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, AccessFieldOnNonStruct) {
    std::string source = R"(
        let a = 1;
        a.foo;
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidTypeAnnotation) {
    std::string source = "let x: int = 10;";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, MismatchedTypeAnnotation) {
    std::string source = "let x: string = 10;";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, StructTypeAnnotation) {
    std::string source = R"(
        struct Foo {}
        let f: Foo = Foo{};
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ValidUnary) {
    std::string source = "let x = !true; let y = -10;";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, InvalidUnary) {
    std::string source = "let x = !123; let y = -\"hello\";";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidFunctionCall) {
    std::string source = R"(
        func add(a: int, b: int) -> int { return a + b; }
        let x = add(1, 2);
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, WrongArityFunctionCall) {
    std::string source = R"(
        func add(a: int, b: int) -> int { return a + b; }
        let x = add(1);
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, WrongTypeFunctionCall) {
    std::string source = R"(
        func add(a: int, b: int) -> int { return a + b; }
        let x = add(1, "hello");
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, CallNonFunction) {
    std::string source = R"(
        let x = 10;
        let y = x();
    )";
    ASSERT_TRUE(resolve(source));
}
