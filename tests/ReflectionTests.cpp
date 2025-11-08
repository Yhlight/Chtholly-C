#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(ReflectionTest, GetFieldCount) {
    std::string source = R"(
        struct Point {
            x: int;
            y: int;
        }
        let p: Point = Point{x: 1, y: 2};
        let count = reflect::get_field_count(p);
    )";
    std::string expected = "const int count = 2;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(ReflectionTest, GetFieldValue) {
    std::string source = R"(
        import reflect;
        struct Point {
            x: int;
            y: string;
        }
        let p: Point = Point{x: 1, y: "hello"};
        let x_val = reflect::get_field_value(p, "x");
    )";
    std::string expected = "const int x_val = p.x;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(ReflectionTest, GetMethodCount) {
    std::string source = R"(
        struct Point {
            x: int;
            y: int;
            func move(dx: int, dy: int) -> void {}
        }
        let p: Point = Point{x: 1, y: 2};
        let count = reflect::get_method_count(p);
    )";
    std::string expected = "const int count = 1;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(ReflectionTest, GetMethods) {
    std::string source = R"(
        import reflect;
        struct Point {
            x: int;
            y: int;
            func move(dx: int, dy: string) -> void {}
        }
        let p: Point = Point{x: 1, y: 2};
        let methods = reflect::get_methods(p);
    )";
    std::string expected = "const std::vector<chtholly_method> methods = std::vector<chtholly_method>{{\"move\", \"void\", {{\"dx\", \"int\"}, {\"dy\", \"std::string\"}}}};";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(ReflectionTest, GetFields) {
    std::string source = R"(
        import reflect;
        struct Point {
            x: int;
            y: string;
        }
        let p: Point = Point{x: 1, y: "hello"};
        let fields = reflect::get_fields(p);
    )";
    std::string expected = "const std::vector<chtholly_field> fields = std::vector<chtholly_field>{{\"x\", \"int\"}, {\"y\", \"std::string\"}};";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
