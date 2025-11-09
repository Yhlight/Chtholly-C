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

TEST(TestGenerics, GenericFunctionWithConstraints) {
    std::string source = R"(
        trait MyTrait {}
        func my_func<T ? MyTrait>(arg: T) {}
    )";
    std::string expected = R"(
        #include <type_traits>
        struct MyTrait {
            virtual ~MyTrait() = default;
        };
        template <typename T>
        void my_func(T arg) {
            static_assert(std::is_base_of<MyTrait, T>::value, "T must implement MyTrait");
        }
    )";
    ASSERT_EQ(normalize(compile(source)), normalize(expected));
}

TEST(TestGenerics, GenericStruct) {
    std::string source = R"(
        struct Point<T> {
            public:
            x: T;
            y: T;
        }
    )";
    std::string expected = R"(
        template <typename T>
        struct Point {
            public:
            T x;
            T y;
        };
    )";
    ASSERT_EQ(normalize(compile(source)), normalize(expected));
}
