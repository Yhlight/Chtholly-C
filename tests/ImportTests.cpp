#include "TestHelpers.h"
#include <gtest/gtest.h>
#include <fstream>

TEST(ImportTests, CanImportAndUseTypesAndFunctions) {
    // Create a temporary module file
    std::ofstream module_file("module.cns");
    module_file << R"(
        struct Point {
            x: int;
            y: int;
        }

        func add(a: int, b: int) -> int {
            return a + b;
        }
    )";
    module_file.close();

    // Create the main file that imports the module
    std::string source = R"(
        import "module.cns";
        import iostream;

        func main() {
            let p = Point{x: 10, y: 20};
            print(add(p.x, p.y));
        }
    )";

    std::string expected_output = "30\n";
    ASSERT_EQ(normalize(compile_and_run(source)), normalize(expected_output));

    // Clean up temporary files
    remove("module.cns");
}
