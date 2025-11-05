#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(UtilTest, StringCastInt) {
    std::string source = R"(
        import util;
        let x: int = 10;
        let s = util::string_cast(x);
    )";
    std::string expected = "const std::string s = std::to_string(x);";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(UtilTest, StringCastDouble) {
    std::string source = R"(
        import util;
        let x: double = 10.5;
        let s = util::string_cast(x);
    )";
    std::string expected = "const std::string s = std::to_string(x);";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(UtilTest, StringCastCustomType) {
    std::string source =
        "import util;"
        "struct Point impl util::to_string {"
        "    x: int;"
        "    y: int;"
        "    func to_string(&self) -> string {"
        "        return \"Point(\" + util::string_cast(self.x) + \", \" + util::string_cast(self.y) + \")\";"
        "    }"
        "}"
        "let p: Point = Point{x: 1, y: 2};"
        "let s = util::string_cast(p);";
    std::string expected = "const std::string s = p.to_string();";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
