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

// OS, Time, Random, Util module tests
TEST_F(ResolverTest, ValidOsExit) {
    std::string source = "os::exit(0);";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, OsEnvWrongArgType) {
    std::string source = "os::env(123);";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, TimeNowWrongArity) {
    std::string source = "time::now(123);";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidRandomRandint) {
    std::string source = "let x: int = random::randint(0, 10);";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, RandomRandWrongArity) {
    std::string source = "random::rand(1);";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidUtilPanic) {
    std::string source = "util::panic(\"test\");";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, UtilUnknownFunction) {
    std::string source = "util::unknown();";
    ASSERT_TRUE(resolve(source));
}

// Reflect module tests
TEST_F(ResolverTest, ValidReflectGetFieldCount) {
    std::string source = R"(
        struct MyStruct { a: int; }
        let s = MyStruct{a: 1};
        let count: int = reflect::get_field_count(s);
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ReflectOnNonStruct) {
    std::string source = R"(
        let x = 10;
        let fields = reflect::get_fields(x);
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ReflectWrongArity) {
    std::string source = R"(
        struct MyStruct { a: int; }
        let s = MyStruct{a: 1};
        let field = reflect::get_field(s);
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ReflectWrongArgType) {
    std::string source = R"(
        struct MyStruct { func foo() {} }
        let s = MyStruct{};
        let method = reflect::get_method(s, 123);
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ReflectUnknownFunction) {
    std::string source = R"(
        struct MyStruct {}
        let s = MyStruct{};
        let val = reflect::unknown_func(s);
    )";
    ASSERT_TRUE(resolve(source));
}

// Meta module tests
TEST_F(ResolverTest, ValidMetaIsInt) {
    std::string source = R"(
        let x = 10;
        let b: bool = meta::is_int(x);
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ValidMetaTypeName) {
    std::string source = R"(
        let x = 10;
        let s: string = meta::type_name(x);
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, MetaWrongArity) {
    std::string source = R"(
        let x = 10;
        let b = meta::is_string(x, x);
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, MetaUnknownFunction) {
    std::string source = R"(
        let x = 10;
        let b = meta::unknown_func(x);
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidSwitch) {
    std::string source = R"(
        let x = 1;
        switch (x) {
            case 1:
                break;
            default:
                break;
        }
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, MismatchedSwitchCaseType) {
    std::string source = R"(
        let x = 1;
        switch (x) {
            case "hello":
                break;
        }
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, BreakOutsideLoopOrSwitch) {
    std::string source = "break;";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, FallthroughOutsideSwitch) {
    std::string source = "fallthrough;";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidLambda) {
    std::string source = R"(
        let f = [](a: int) -> int { return a; };
        let x: int = f(10);
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, MismatchedLambdaReturnType) {
    std::string source = R"(
        let f = [](a: int) -> int { return "hello"; };
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidBorrow) {
    std::string source = R"(
        let x: int = 10;
        let y: &int = &x;
    )";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, MismatchedBorrowType) {
    std::string source = R"(
        let x: int = 10;
        let y: &string = &x;
    )";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidTypeCast) {
    std::string source = "let x: string = type_cast<string>(10);";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, TypeComparisonLogic) {
    // Basic type mismatch
    ASSERT_TRUE(resolve("let x: int = \"hello\";"));

    // Array type mismatch
    ASSERT_TRUE(resolve("let x: array[int] = [\"hello\"];"));

    // Function return type mismatch
    ASSERT_TRUE(resolve("func f() -> int { return \"hello\"; }"));

    // Function parameter type mismatch
    ASSERT_TRUE(resolve("func f(a: int) {} f(\"hello\");"));

    // Struct field type mismatch
    ASSERT_TRUE(resolve("struct S { a: int; } let s = S{a: \"hello\"};"));
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
