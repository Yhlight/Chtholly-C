#include <gtest/gtest.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <array>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

TEST(TranspilerTest, SimpleLetStatement) {
    std::string source = "let a = 10;";
    std::ofstream out("test.cns");
    out << source;
    out.close();

    std::string expected_output = "#include <iostream>\n"
                                  "#include <string>\n\n"
                                  "int main() {\n"
                                  "    const auto a = 10;\n"
                                  "    return 0;\n"
                                  "}\n";

    std::string output = exec("./build/chtholly test.cns");
    EXPECT_EQ(output, expected_output);
}
