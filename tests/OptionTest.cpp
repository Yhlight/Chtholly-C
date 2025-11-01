#include "TestHelpers.h"
#include <gtest/gtest.h>

TEST(OptionTest, OptionDeclarationAndInitializationWithNone) {
    std::string source = "let a: option<int> = none;";
    std::string expected = R"(
        #include <string>
        #include <vector>
        #include <array>
        #include <optional>
        const std::optional<int> a = std::nullopt;
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OptionTest, OptionDeclarationAndInitializationWithValue) {
    std::string source = "let a: option<int> = 10;";
    std::string expected = R"(
        #include <string>
        #include <vector>
        #include <array>
        #include <optional>
        const std::optional<int> a = 10;
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OptionTest, UnwrapCall) {
    std::string source = R"(
        let a: option<int> = 10;
        let b: int = a.unwrap();
    )";
    std::string expected = R"(
        #include <string>
        #include <vector>
        #include <array>
        #include <optional>
        const std::optional<int> a = 10;
        const int b = a.value();
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OptionTest, UnwrapOrCall) {
    std::string source = R"(
        let a: option<int> = none;
        let b: int = a.unwrap_or(20);
    )";
    std::string expected = R"(
        #include <string>
        #include <vector>
        #include <array>
        #include <optional>
        const std::optional<int> a = std::nullopt;
        const int b = a.value_or(20);
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(OptionTest, TypeMismatchError) {
    std::string source = "let a: option<int> = \"hello\";";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), "Sema error");
}

TEST(OptionTest, NoneWithoutTypeAnnotationError) {
    std::string source = "let a = none;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), "Sema error");
}
