#include <gtest/gtest.h>
#include "TestHelpers.h"

class MathTest : public ::testing::Test {};

TEST_F(MathTest, SqrtFunction) {
    std::string source = "import math; let a = math::sqrt(16.0);";
    std::string expected = "const double a = std::sqrt(16.0);";
    ASSERT_NE(compile(source).find(expected), std::string::npos);
}

TEST_F(MathTest, PowFunction) {
    std::string source = "import math; let b = math::pow(2.0, 3.0);";
    std::string expected = "const double b = std::pow(2.0, 3.0);";
    ASSERT_NE(compile(source).find(expected), std::string::npos);
}

TEST_F(MathTest, TrigFunctions) {
    std::string source = "import math; let c = math::sin(0.0); let d = math::cos(0.0); let e = math::tan(0.0);";
    ASSERT_NE(compile(source).find("const double c = std::sin(0.0);"), std::string::npos);
    ASSERT_NE(compile(source).find("const double d = std::cos(0.0);"), std::string::npos);
    ASSERT_NE(compile(source).find("const double e = std::tan(0.0);"), std::string::npos);
}

TEST_F(MathTest, LogFunctions) {
    std::string source = "import math; let f = math::log(1.0); let g = math::log10(1.0);";
    ASSERT_NE(compile(source).find("const double f = std::log(1.0);"), std::string::npos);
    ASSERT_NE(compile(source).find("const double g = std::log10(1.0);"), std::string::npos);
}

TEST_F(MathTest, AbsFunction) {
    std::string source = "import math; let h = math::abs(-1.0);";
    std::string expected = "const double h = std::abs(-1.0);";
    ASSERT_NE(compile(source).find(expected), std::string::npos);
}

TEST_F(MathTest, CeilFunction) {
    std::string source = "import math; let i = math::ceil(3.14);";
    std::string expected = "const double i = std::ceil(3.14);";
    ASSERT_NE(compile(source).find(expected), std::string::npos);
}

TEST_F(MathTest, FloorFunction) {
    std::string source = "import math; let j = math::floor(3.14);";
    std::string expected = "const double j = std::floor(3.14);";
    ASSERT_NE(compile(source).find(expected), std::string::npos);
}

TEST_F(MathTest, RoundFunction) {
    std::string source = "import math; let k = math::round(3.5);";
    std::string expected = "const double k = std::round(3.5);";
    ASSERT_NE(compile(source).find(expected), std::string::npos);
}

TEST_F(MathTest, Constants) {
    std::string source = "import math; let pi = math::PI; let e = math::E;";
    ASSERT_NE(compile(source).find("const double pi = M_PI;"), std::string::npos);
    ASSERT_NE(compile(source).find("const double e = M_E;"), std::string::npos);
}
