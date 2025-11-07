#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(MetaTest, IsInt) {
    std::string source = R"(
        let x: int = 10;
        let y: string = "hello";
        let is_x_int = meta::is_int(x);
        let is_y_int = meta::is_int(y);
    )";
    std::string expected = "const bool is_x_int = true;\nconst bool is_y_int = false;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(MetaTest, IsArray) {
    std::string source = R"(
        let x: array[int] = [1, 2, 3];
        let y: array[int; 3] = [1, 2, 3];
        let z: int = 10;
        let is_x_array = meta::is_array(x);
        let is_y_array = meta::is_array(y);
        let is_z_array = meta::is_array(z);
    )";
    std::string expected = "const bool is_x_array = true;\nconst bool is_y_array = true;\nconst bool is_z_array = false;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(MetaTest, IsUInt) {
    std::string source = R"(
        let x: uint = 10;
        let y: int = -10;
        let is_x_uint = meta::is_uint(x);
        let is_y_uint = meta::is_uint(y);
    )";
    std::string expected = "const bool is_x_uint = true;\nconst bool is_y_uint = false;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(MetaTest, IsDouble) {
    std::string source = R"(
        let x: double = 10.5;
        let y: int = 10;
        let is_x_double = meta::is_double(x);
        let is_y_double = meta::is_double(y);
    )";
    std::string expected = "const bool is_x_double = true;\nconst bool is_y_double = false;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(MetaTest, IsChar) {
    std::string source = R"(
        let x: char = 'a';
        let y: string = "a";
        let is_x_char = meta::is_char(x);
        let is_y_char = meta::is_char(y);
    )";
    std::string expected = "const bool is_x_char = true;\nconst bool is_y_char = false;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(MetaTest, IsBool) {
    std::string source = R"(
        let x: bool = true;
        let y: int = 1;
        let is_x_bool = meta::is_bool(x);
        let is_y_bool = meta::is_bool(y);
    )";
    std::string expected = "const bool is_x_bool = true;\nconst bool is_y_bool = false;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(MetaTest, IsString) {
    std::string source = R"(
        let x: string = "hello";
        let y: char = 'h';
        let is_x_string = meta::is_string(x);
        let is_y_string = meta::is_string(y);
    )";
    std::string expected = "const bool is_x_string = true;\nconst bool is_y_string = false;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(MetaTest, IsStruct) {
    std::string source = R"(
        struct Point { x: int; y: int; }
        let p: Point = Point{x: 1, y: 2};
        let x: int = 10;
        let is_p_struct = meta::is_struct(p);
        let is_x_struct = meta::is_struct(x);
    )";
    std::string expected = "const bool is_p_struct = true;\nconst bool is_x_struct = false;";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
