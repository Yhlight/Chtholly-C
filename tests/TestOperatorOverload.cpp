#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(TestOperatorOverload, Add) {
    std::string source = R"(
        import operator;
        struct Point impl operator::add {
            x: int;
            y: int;
            add(other: Point) -> Point {
                return Point{x: self.x + other.x, y: self.y + other.y};
            }
        }
        func main() {
            let p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            let p3 = p1 + p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_add {
int x;
int y;
Point add(Point other) override {
return Point{.x = this->x + other.x, .y = this->y + other.y};
}
};
void main() {
const Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
const Point p3 = p1.add(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, PrefixAdd) {
    std::string source = R"(
        import operator;
        struct Point impl operator::prefix_add {
            x: int;
            y: int;
            prefix_add(&mut self) {
                self.x = self.x + 1;
                self.y = self.y + 1;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            ++p1;
        }
    )";
    std::string expected = R"(struct Point : public operator_prefix_add {
int x;
int y;
void prefix_add() override {
this->x = this->x + 1;
this->y = this->y + 1;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
p1.prefix_add();
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, PostfixAdd) {
    std::string source = R"(
        import operator;
        struct Point impl operator::postfix_add {
            x: int;
            y: int;
            postfix_add(&mut self) {
                self.x = self.x + 1;
                self.y = self.y + 1;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            p1++;
        }
    )";
    std::string expected = R"(struct Point : public operator_postfix_add {
int x;
int y;
void postfix_add() override {
this->x = this->x + 1;
this->y = this->y + 1;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
p1.postfix_add();
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, PrefixSub) {
    std::string source = R"(
        import operator;
        struct Point impl operator::prefix_sub {
            x: int;
            y: int;
            prefix_sub(&mut self) {
                self.x = self.x - 1;
                self.y = self.y - 1;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            --p1;
        }
    )";
    std::string expected = R"(struct Point : public operator_prefix_sub {
int x;
int y;
void prefix_sub() override {
this->x = this->x - 1;
this->y = this->y - 1;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
p1.prefix_sub();
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, PostfixSub) {
    std::string source = R"(
        import operator;
        struct Point impl operator::postfix_sub {
            x: int;
            y: int;
            postfix_sub(&mut self) {
                self.x = self.x - 1;
                self.y = self.y - 1;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            p1--;
        }
    )";
    std::string expected = R"(struct Point : public operator_postfix_sub {
int x;
int y;
void postfix_sub() override {
this->x = this->x - 1;
this->y = this->y - 1;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
p1.postfix_sub();
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, CompoundSubtract) {
    std::string source = R"(
        import operator;
        struct Point impl operator::assign_sub {
            x: int;
            y: int;
            assign_sub(&mut self, other: Point) {
                self.x = self.x - other.x;
                self.y = self.y - other.y;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            p1 -= p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_assign_sub {
int x;
int y;
void assign_sub(Point other) override {
this->x = this->x - other.x;
this->y = this->y - other.y;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
p1.assign_sub(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, CompoundMultiply) {
    std::string source = R"(
        import operator;
        struct Point impl operator::assign_mul {
            x: int;
            y: int;
            assign_mul(&mut self, other: Point) {
                self.x = self.x * other.x;
                self.y = self.y * other.y;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            p1 *= p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_assign_mul {
int x;
int y;
void assign_mul(Point other) override {
this->x = this->x * other.x;
this->y = this->y * other.y;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
p1.assign_mul(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, CompoundDivision) {
    std::string source = R"(
        import operator;
        struct Point impl operator::assign_div {
            x: int;
            y: int;
            assign_div(&mut self, other: Point) {
                self.x = self.x / other.x;
                self.y = self.y / other.y;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            p1 /= p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_assign_div {
int x;
int y;
void assign_div(Point other) override {
this->x = this->x / other.x;
this->y = this->y / other.y;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
p1.assign_div(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, CompoundModulo) {
    std::string source = R"(
        import operator;
        struct Point impl operator::assign_mod {
            x: int;
            y: int;
            assign_mod(&mut self, other: Point) {
                self.x = self.x % other.x;
                self.y = self.y % other.y;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            p1 %= p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_assign_mod {
int x;
int y;
void assign_mod(Point other) override {
this->x = this->x % other.x;
this->y = this->y % other.y;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
p1.assign_mod(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, CompoundAdd) {
    std::string source = R"(
        import operator;
        struct Point impl operator::assign_add {
            x: int;
            y: int;
            assign_add(&mut self, other: Point) {
                self.x = self.x + other.x;
                self.y = self.y + other.y;
            }
        }
        func main() {
            mut p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            p1 += p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_assign_add {
int x;
int y;
void assign_add(Point other) override {
this->x = this->x + other.x;
this->y = this->y + other.y;
}
};
void main() {
Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
p1.assign_add(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, Subtract) {
    std::string source = R"(
        import operator;
        struct Point impl operator::sub {
            x: int;
            y: int;
            sub(other: Point) -> Point {
                return Point{x: self.x - other.x, y: self.y - other.y};
            }
        }
        func main() {
            let p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            let p3 = p1 - p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_sub {
int x;
int y;
Point sub(Point other) override {
return Point{.x = this->x - other.x, .y = this->y - other.y};
}
};
void main() {
const Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
const Point p3 = p1.sub(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, Multiply) {
    std::string source = R"(
        import operator;
        struct Point impl operator::mul {
            x: int;
            y: int;
            mul(other: Point) -> Point {
                return Point{x: self.x * other.x, y: self.y * other.y};
            }
        }
        func main() {
            let p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            let p3 = p1 * p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_mul {
int x;
int y;
Point mul(Point other) override {
return Point{.x = this->x * other.x, .y = this->y * other.y};
}
};
void main() {
const Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
const Point p3 = p1.mul(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, Division) {
    std::string source = R"(
        import operator;
        struct Point impl operator::div {
            x: int;
            y: int;
            div(other: Point) -> Point {
                return Point{x: self.x / other.x, y: self.y / other.y};
            }
        }
        func main() {
            let p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            let p3 = p1 / p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_div {
int x;
int y;
Point div(Point other) override {
return Point{.x = this->x / other.x, .y = this->y / other.y};
}
};
void main() {
const Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
const Point p3 = p1.div(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestOperatorOverload, Comparison) {
    std::string source = R"(
        import operator;
        struct Point impl operator::less, operator::greater {
            x: int;
            y: int;
            less(other: Point) -> bool {
                return self.x < other.x;
            }
            greater(other: Point) -> bool {
                return self.x > other.x;
            }
        }
        func main() {
            let p1 = Point{x: 1, y: 2};
            let p2 = Point{x: 3, y: 4};
            let b1 = p1 < p2;
            let b2 = p1 > p2;
        }
    )";
    std::string expected = R"(struct Point : public operator_less, public operator_greater {
int x;
int y;
bool less(Point other) override {
return this->x < other.x;
}
bool greater(Point other) override {
return this->x > other.x;
}
};
void main() {
const Point p1 = Point{.x = 1, .y = 2};
const Point p2 = Point{.x = 3, .y = 4};
const bool b1 = p1.less(p2);
const bool b2 = p1.greater(p2);
}
)";
    ASSERT_EQ(compile(source), expected);
}
