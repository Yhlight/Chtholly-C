#include "gtest/gtest.h"
#include "TestHelpers.h"
#include <fstream>

TEST(ImportTest, SimpleImport) {
    std::ofstream helper_file("helper.cns");
    helper_file << R"(
        func add(a: int, b: int) -> int {
            return a + b;
        }
    )";
    helper_file.close();

    std::string source = R"(
        import "helper.cns";
        func main() -> int {
            return add(2, 3);
        }
    )";

    std::string expected = R"(
        #include <string>
        #include <vector>
        #include <array>

        auto add(int a, int b) -> int {
            return (a + b);
        }

        auto main() -> int {
            return add(2, 3);
        }
    )";

    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}
