#include <gtest/gtest.h>
#include "Sema.h"
#include "Parser.h"
#include "Lexer.h"

using namespace chtholly;

// Helper function to parse a source string and run semantic analysis
Sema analyzeSource(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Sema sema;
    sema.analyze(stmts);
    return sema;
}

TEST(SemaTest, VarDeclSuccess) {
    Sema sema = analyzeSource("let a = 10;");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, VarRedefinitionError) {
    Sema sema = analyzeSource("let a = 10; let a = 20;");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, VarDeclWithTypeAnnotation) {
    Sema sema = analyzeSource("let a: int = 10;");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, VarInferenceFromLiteral) {
    Sema sema = analyzeSource("let a = \"hello\";");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, UndeclaredVariable) {
    Sema sema = analyzeSource("let a = b;");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, BinaryNumericSuccess) {
    Sema sema = analyzeSource("let a = 1 + 2 * 3;");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, UnaryBoolSuccess) {
    Sema sema = analyzeSource("let a = !true;");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, BinaryTypeError) {
    Sema sema = analyzeSource("let a = 1 + \"hello\";");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, UnaryTypeError) {
    Sema sema = analyzeSource("let a = !1;");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, VarDeclTypeMismatch) {
    Sema sema = analyzeSource("let a: int = \"hello\";");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, AssignmentToImmutableError) {
    Sema sema = analyzeSource("let x = 1; x = 2;");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, AssignmentSuccess) {
    Sema sema = analyzeSource("mut x = 1; x = 2;");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, AssignmentTypeMismatchError) {
    Sema sema = analyzeSource("mut x = 1; x = \"hello\";");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, IfConditionTypeError) {
    Sema sema = analyzeSource("if (1) {}");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, ForConditionTypeError) {
    Sema sema = analyzeSource("for (; 1; ) {}");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, IfConditionSuccess) {
    Sema sema = analyzeSource("if (true) {}");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, FuncDeclSuccess) {
    Sema sema = analyzeSource("func add(a: int, b: int) -> int { return a + b; }");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, FuncRedefinitionError) {
    Sema sema = analyzeSource("func a() {} func a() {}");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, ReturnTypeMismatchError) {
    Sema sema = analyzeSource("func a() -> int { return \"hello\"; }");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, ReturnInVoidFuncError) {
    Sema sema = analyzeSource("func a() { return 1; }");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, CallSuccess) {
    Sema sema = analyzeSource("func add(a: int, b: int) -> int { return a + b; } let c = add(1, 2);");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, CallNonFunctionError) {
    Sema sema = analyzeSource("let a = 1; let b = a();");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, CallWrongArgumentCountError) {
    Sema sema = analyzeSource("func a(b: int) {} a();");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, CallWrongArgumentTypeError) {
    Sema sema = analyzeSource("func a(b: int) {} a(\"hello\");");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, LambdaSuccess) {
    Sema sema = analyzeSource("let f = [](a: int) -> int { return a; };");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, LambdaReturnTypeMismatch) {
    Sema sema = analyzeSource("let f = []() -> int { return \"hello\"; };");
    EXPECT_TRUE(sema.hadError());
}
