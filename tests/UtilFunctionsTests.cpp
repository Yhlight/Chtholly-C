#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(UtilFunctionsTest, SerializeDeserialize) {
    std::string source = R"(
        import util;
        struct Point {
            x: int;
            y: int;
        }
        let p1 = Point{x: 10, y: 20};
        let s = util::serialize(p1);
        let p2 = util::deserialize<Point>(s);
    )";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find("chtholly_serialize(p1)"), std::string::npos);
    ASSERT_NE(transpiled.find("chtholly_deserialize<Point>(s)"), std::string::npos);
}

TEST(UtilFunctionsTest, UniqueId) {
    std::string source = R"(
        import util;
        let id1 = util::unique_id();
        let id2 = util::unique_id();
    )";
    std::string transpiled = compile(source);
    ASSERT_NE(transpiled.find("chtholly_unique_id()"), std::string::npos);
}
