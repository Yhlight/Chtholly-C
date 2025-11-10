#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(TestBitwiseOperators, BitwiseAnd) {
    std::string code = R"(
        import operator;
        struct BitwiseValue impl operator::bit_and {
            value: int;

            public:
                bit_and(self, other: BitwiseValue) -> BitwiseValue {
                    return BitwiseValue{ value: self.value & other.value };
                }
        }

        func main() -> int {
            mut a = BitwiseValue{ value: 6 }; // 110
            let b = BitwiseValue{ value: 5 }; // 101
            let c = a & b;
            return c.value; // Expected: 4 (100)
        }
    )";
    int result = chtholly::run(code);
    ASSERT_EQ(result, 4);
}

TEST(TestBitwiseOperators, BitwiseOr) {
    std::string code = R"(
        import operator;
        struct BitwiseValue impl operator::bit_or {
            value: int;

            public:
                bit_or(self, other: BitwiseValue) -> BitwiseValue {
                    return BitwiseValue{ value: self.value | other.value };
                }
        }

        func main() -> int {
            mut a = BitwiseValue{ value: 6 }; // 110
            let b = BitwiseValue{ value: 5 }; // 101
            let c = a | b;
            return c.value; // Expected: 7 (111)
        }
    )";
    int result = chtholly::run(code);
    ASSERT_EQ(result, 7);
}

TEST(TestBitwiseOperators, BitwiseXor) {
    std::string code = R"(
        import operator;
        struct BitwiseValue impl operator::bit_xor {
            value: int;

            public:
                bit_xor(self, other: BitwiseValue) -> BitwiseValue {
                    return BitwiseValue{ value: self.value ^ other.value };
                }
        }

        func main() -> int {
            mut a = BitwiseValue{ value: 6 }; // 110
            let b = BitwiseValue{ value: 5 }; // 101
            let c = a ^ b;
            return c.value; // Expected: 3 (011)
        }
    )";
    int result = chtholly::run(code);
    ASSERT_EQ(result, 3);
}

TEST(TestBitwiseOperators, BitwiseNot) {
    std::string code = R"(
        import operator;
        struct BitwiseValue impl operator::bit_not {
            value: int;

            public:
                bit_not(self) -> BitwiseValue {
                    return BitwiseValue{ value: ~self.value };
                }
        }

        func main() -> uint {
            mut a = BitwiseValue{ value: 6 };
            let b = ~a;
            return type_cast<uint>(b.value);
        }
    )";
    int result = chtholly::run(code);
    ASSERT_EQ(result, 249);
}

TEST(TestBitwiseOperators, ShiftLeft) {
    std::string code = R"(
        import operator;
        struct BitwiseValue impl operator::shl {
            value: int;

            public:
                shl(self, other: int) -> BitwiseValue {
                    return BitwiseValue{ value: self.value << other };
                }
        }

        func main() -> int {
            mut a = BitwiseValue{ value: 6 }; // 110
            let c = a << 2;
            return c.value; // Expected: 24 (11000)
        }
    )";
    int result = chtholly::run(code);
    ASSERT_EQ(result, 24);
}

TEST(TestBitwiseOperators, ShiftRight) {
    std::string code = R"(
        import operator;
        struct BitwiseValue impl operator::shr {
            value: int;

            public:
                shr(self, other: int) -> BitwiseValue {
                    return BitwiseValue{ value: self.value >> other };
                }
        }

        func main() -> int {
            mut a = BitwiseValue{ value: 6 }; // 110
            let c = a >> 1;
            return c.value; // Expected: 3 (011)
        }
    )";
    int result = chtholly::run(code);
    ASSERT_EQ(result, 3);
}
