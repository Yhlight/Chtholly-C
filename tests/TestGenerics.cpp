#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(TestGenerics, GenericFunction) {
    std::string source = R"(
        func swap<T>(a: &mut T, b: &mut T) {
            let temp = *a;
            *a = *b;
            *b = temp;
        }
    )";
    std::string expected = R"(
        template <typename T>
        void swap(T& a, T& b) {
            const auto temp = *a;
            *a = *b;
            *b = temp;
        }
    )";
    ASSERT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(TestGenerics, GenericStruct) {
    std::string source = R"(
        struct Point<T> {
            x: T;
            y: T;
        }
    )";
    std::string expected = R"(
        template <typename T>
        struct Point {
            T x;
            T y;
        };
    )";
    ASSERT_EQ(normalize(compile(source)), normalize(expected));
}
