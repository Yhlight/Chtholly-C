#include <gtest/gtest.h>
#include "TestHelpers.h"
#include <cmath>

TEST(MathTest, AllFunctions) {
    std::string source = R"(
        import math;
        import iostream;

        let a = math.sqrt(16.0);
        print(a);
        let b = math.pow(2.0, 3.0);
        print(b);
        let c = math.sin(0.0);
        print(c);
        let d = math.cos(0.0);
        print(d);
        let e = math.tan(0.0);
        print(e);
        let f = math.log(1.0);
        print(f);
        let g = math.log10(1.0);
        print(g);
        let h = math.abs(-1.0);
        print(h);
    )";

    std::string expected_transpilation = R"(
#include <iostream>
#include <cmath>
const double a = std::sqrt(16.0);
std::cout << a << std::endl;
const double b = std::pow(2.0, 3.0);
std::cout << b << std::endl;
const double c = std::sin(0.0);
std::cout << c << std::endl;
const double d = std::cos(0.0);
std::cout << d << std::endl;
const double e = std::tan(0.0);
std::cout << e << std::endl;
const double f = std::log(1.0);
std::cout << f << std::endl;
const double g = std::log10(1.0);
std::cout << g << std::endl;
const double h = std::abs(-1.0);
std::cout << h << std::endl;
)";

    std::string transpiled = compile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    ASSERT_NE(transpiled.find("const double a = std::sqrt(16.0);"), std::string::npos);
    ASSERT_NE(transpiled.find("const double b = std::pow(2.0, 3.0);"), std::string::npos);
    ASSERT_NE(transpiled.find("const double c = std::sin(0.0);"), std::string::npos);
    ASSERT_NE(transpiled.find("const double d = std::cos(0.0);"), std::string::npos);
    ASSERT_NE(transpiled.find("const double e = std::tan(0.0);"), std::string::npos);
    ASSERT_NE(transpiled.find("const double f = std::log(1.0);"), std::string::npos);
    ASSERT_NE(transpiled.find("const double g = std::log10(1.0);"), std::string::npos);
    ASSERT_NE(transpiled.find("const double h = std::abs(-1.0);"), std::string::npos);
}
