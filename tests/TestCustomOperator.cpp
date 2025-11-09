#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(TestCustomOperator, BinaryOperator) {
    std::string source = R"(
        import operator;

        struct Vector impl operator::binary {
            public:
                x: int;
                y: int;

                binary(&self, op: string, other: Vector) -> int {
                    if (op == "**") {
                        return self.x * other.x + self.y * other.y;
                    }
                    return 0;
                }
        }

        func main() {
            let v1 = Vector{x: 2, y: 3};
            let v2 = Vector{x: 4, y: 5};
            let dot_product: int = v1 ** v2;
        }
    )";

    std::string expected = R"(#include <string>
struct Vector {
public:
int x;
int y;
int binary(std::string op, Vector other) {
if (op == "**") {
return this->x * other.x + this->y * other.y;
}
return 0;
}
};
void main() {
const Vector v1 = Vector{.x = 2, .y = 3};
const Vector v2 = Vector{.x = 4, .y = 5};
const int dot_product = v1.binary("**", v2);
}
)";

    ASSERT_EQ(compile(source), expected);
}
