#include "TestHelpers.h"
#include <gtest/gtest.h>

const std::string PREAMBLE = "#include <string>\n#include <vector>\n#include <array>\n";

TEST(OwnershipTest, ImmutableBorrow) {
    std::string source = "let a = 10; let b = &a;";
    std::string expected = PREAMBLE + "const int a = 10; const int* b = &(a);";
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
