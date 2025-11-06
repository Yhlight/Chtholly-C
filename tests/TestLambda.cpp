#include <gtest/gtest.h>
#include "TestHelpers.h"

std::string transpile(const std::string& source);

TEST(TestLambda, SimpleLambda) {
    std::string source = R"(
        let add = [](a: int, b: int) -> int { return a + b; };
    )";
    std::string expected = "conststd::function<int(int,int)>add=[](inta,intb)->int{returna+b;};";
    std::string transpiled = transpile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
