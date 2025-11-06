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
    std::string expected = R"(struct Point {
int x;
int y;
Point add(Point other) {
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
    std::string expected = R"(struct Point {
int x;
int y;
void assign_add(Point other) {
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
    std::string expected = R"(struct Point {
int x;
int y;
Point sub(Point other) {
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
    std::string expected = R"(struct Point {
int x;
int y;
Point mul(Point other) {
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
    std::string expected = R"(struct Point {
int x;
int y;
Point div(Point other) {
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
    std::string expected = R"(struct Point {
int x;
int y;
bool less(Point other) {
return this->x < other.x;
}
bool greater(Point other) {
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
