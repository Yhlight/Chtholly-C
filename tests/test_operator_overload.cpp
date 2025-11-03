#include "gtest/gtest.h"
#include "../src/Chtholly.h"
#include "../src/Transpiler.h"
#include <string>
#include "../src/Error.h"

TEST(OperatorOverloadTest, SimpleAddition) {
    std::string source = R"(
        struct Point impl add, sub {
            let x: int;
            let y: int;

            func add(other: Point) -> Point {
                return Point {
                    x: self.x + other.x,
                    y: self.y + other.y
                };
            }
            func sub(other: Point) -> Point {
                return Point {
                    x: self.x - other.x,
                    y: self.y - other.y
                };
            }
        }

        let p1 = Point { x: 1, y: 2 };
        let p2 = Point { x: 3, y: 4 };
        let p3 = p1 + p2;
        let p4 = p1 - p2;
    )";

    Chtholly chtholly;
    auto statements = chtholly.run(source, true);
    Transpiler transpiler;
    std::string result = transpiler.transpile(statements);

    std::string expected = R"(#include <iostream>
#include <variant>
#include <string>

struct Point {
    const int x;
    const int y;

    Point add(Point other) {
        return Point{this->x + other.x, this->y + other.y};
    }
    Point sub(Point other) {
        return Point{this->x - other.x, this->y - other.y};
    }
};

int main() {
    const Point p1 = {1, 2};
    const Point p2 = {3, 4};
    const Point p3 = p1.add(p2);
    const Point p4 = p1.sub(p2);
    return 0;
}
)";

    ASSERT_EQ(result, expected);
}

TEST(OperatorOverloadTest, MissingOverload) {
    std::string source = R"(
        struct Point {
            let x: int;
            let y: int;
        }

        let p1 = Point { x: 1, y: 2 };
        let p2 = Point { x: 3, y: 4 };
        let p3 = p1 * p2;
    )";

    Chtholly chtholly;
    ErrorReporter::hadError = false;
    auto statements = chtholly.run(source, true);
    Transpiler transpiler;
    transpiler.transpile(statements);
    ASSERT_TRUE(ErrorReporter::hadError);
}
