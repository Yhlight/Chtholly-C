#include <iostream>
#include <vector>
#include <string>
#include <functional>

namespace TinyTest {
    struct TestCase {
        std::string name;
        std::function<void()> test_fn;
    };

    std::vector<TestCase> test_cases;
    int tests_passed = 0;
    int tests_failed = 0;

    void add_test(const std::string& name, std::function<void()> test_fn) {
        test_cases.push_back({name, test_fn});
    }

    void run_tests() {
        std::cout << "Running tests..." << std::endl;
        for (const auto& test_case : test_cases) {
            try {
                test_case.test_fn();
                std::cout << "[PASSED] " << test_case.name << std::endl;
                tests_passed++;
            } catch (const std::exception& e) {
                std::cout << "[FAILED] " << test_case.name << " - " << e.what() << std::endl;
                tests_failed++;
            }
        }
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Tests passed: " << tests_passed << ", Tests failed: " << tests_failed << std::endl;
    }

    template<typename T, typename U>
    void assert_equal(const T& a, const U& b, const std::string& message) {
        if (a != b) {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }
}

// Example test case
void test_addition() {
    TinyTest::assert_equal(1 + 1, 2, "1 + 1 should equal 2");
}

int main() {
    TinyTest::add_test("test_addition", test_addition);
    TinyTest::run_tests();
    return TinyTest::tests_failed;
}
