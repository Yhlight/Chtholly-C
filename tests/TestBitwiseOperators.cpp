#include <gtest/gtest.h>
#include "TestHelpers.h"

class BitwiseOperatorsTest : public TranspilerTest {};

TEST_F(BitwiseOperatorsTest, BitwiseAnd) {
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
    int result = run(code);
    ASSERT_EQ(result, 4);
}

TEST_F(BitwiseOperatorsTest, BitwiseOr) {
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
    int result = run(code);
    ASSERT_EQ(result, 7);
}

TEST_F(BitwiseOperatorsTest, BitwiseXor) {
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
    int result = run(code);
    ASSERT_EQ(result, 3);
}

TEST_F(BitwiseOperatorsTest, BitwiseNot) {
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
    int result = run(code);
    ASSERT_EQ(result, 249);
}

TEST_F(BitwiseOperatorsTest, ShiftLeft) {
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
    int result = run(code);
    ASSERT_EQ(result, 24);
}

TEST_F(BitwiseOperatorsTest, ShiftRight) {
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
    int result = run(code);
    ASSERT_EQ(result, 3);
}
