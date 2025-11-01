#include "TestHelpers.h"
#include <gtest/gtest.h>

const std::string PREAMBLE = "#include <string>\n#include <vector>\n#include <array>\n";

TEST(OwnershipTest, ImmutableBorrow) {
    std::string source = "let a = 10; let b = &a;";
    std::string expected = PREAMBLE + "const int a = 10; const int* b = &(a);";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, MoveSemantics_MoveByInitialization) {
    std::string source = R"(
        let s1: string = "hello";
        let s2: string = s1;
    )";
    std::string expected = PREAMBLE + R"(
        #include <utility>
        const std::string s1 = "hello";
        const std::string s2 = std::move(s1);
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, MoveSemantics_UseAfterMoveInitialization) {
    std::string source = R"(
        let s1: string = "hello";
        let s2: string = s1;
        let s3: string = s1;
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), "Sema error");
}

TEST(OwnershipTest, MoveSemantics_MoveByFunctionCall) {
    std::string source = R"(
        func take_string(s: string) {}
        let s1: string = "hello";
        take_string(s1);
    )";
    std::string expected = PREAMBLE + R"(
        #include <utility>
        auto take_string(std::string s) {
        }
        const std::string s1 = "hello";
        take_string(std::move(s1));
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, MoveSemantics_UseAfterMoveFunctionCall) {
    std::string source = R"(
        func take_string(s: string) {}
        let s1: string = "hello";
        take_string(s1);
        take_string(s1);
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), "Sema error");
}

TEST(OwnershipTest, MoveSemantics_MoveByReturn) {
    std::string source = R"(
        func produce_string() -> string {
            let s: string = "hello";
            return s;
        }
    )";
    std::string expected = PREAMBLE + R"(
        #include <utility>
        auto produce_string() -> std::string {
            const std::string s = "hello";
            return std::move(s);
        }
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, MoveSemantics_AssignmentResetsOwnership) {
    std::string source = R"(
        mut s: string = "one";
        let s2: string = s;
        s = "two";
        let s3: string = s;
    )";
    std::string expected = PREAMBLE + R"(
        #include <utility>
        std::string s = "one";
        const std::string s2 = std::move(s);
        s = "two";
        const std::string s3 = std::move(s);
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, CopyableTypesAreNotMoved) {
    std::string source = R"(
        let x: int = 1;
        let y: int = x;
        let z: int = x;
    )";
    std::string expected = PREAMBLE + R"(
        const int x = 1;
        const int y = x;
        const int z = x;
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, MutableBorrowOfMutable) {
    std::string source = "mut a = 10; let b = &mut a;";
    std::string expected = PREAMBLE + "int a = 10; int* b = &(a);";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, MutableBorrowOfImmutable) {
    std::string source = "let a = 10; let b = &mut a;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), "Sema error");
}

TEST(OwnershipTest, Dereference) {
    std::string source = "let a = 10; let b = &a; let c = *b;";
    std::string expected = PREAMBLE + "const int a = 10; const int* b = &(a); const int c = *(b);";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, AssignmentToDereferencedMutableBorrow) {
    std::string source = "mut a = 10; let b = &mut a; *b = 20;";
    std::string expected = PREAMBLE + "int a = 10; int* b = &(a); *(b) = 20;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OwnershipTest, AssignmentToDereferencedImmutableBorrow) {
    std::string source = "mut a = 10; let b = &a; *b = 20;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), "Sema error");
}

TEST(OwnershipTest, ReferenceTypeAnnotation) {
    std::string source = "let a: &int = &10;";
    std::string expected = PREAMBLE + "const int* a = &(10);";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}
