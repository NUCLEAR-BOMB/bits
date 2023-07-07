#include <gtest/gtest.h>
#include <bits.hpp>

namespace {

struct operators : ::testing::Test {
	int value = 1;
	const int const_value = 2;
	static constexpr int constexpr_value = 3;
};

TEST_F(operators, assigment) {
	bits{value} = 2;
	EXPECT_EQ(value, 2);
}

TEST_F(operators, equal) {
	EXPECT_TRUE(bits{value} == 1);
	EXPECT_TRUE(bits{const_value} == 2);
	static_assert(bits{constexpr_value} == 3);

	EXPECT_TRUE(1 == bits{value});
	EXPECT_TRUE(2 == bits{const_value});
	static_assert(3 == bits{constexpr_value});

	EXPECT_TRUE(bits{value} == bits{value});
	EXPECT_TRUE(bits{const_value} == bits{const_value});
	static_assert(bits{constexpr_value} == bits{constexpr_value});

	EXPECT_FALSE(bits{value} == bits{const_value});
	EXPECT_FALSE(bits{const_value} == bits{value});
	EXPECT_FALSE(bits{value} == bits{constexpr_value});

	EXPECT_TRUE(bits{1.f} == bits{1.f});

	int array_1[8]{1, 0};
	int array_2[8]{0, 1};
	EXPECT_TRUE(bits{array_1} < bits{array_2});
}

TEST_F(operators, not_equal) {
	EXPECT_FALSE(bits{value} != 1);
	EXPECT_FALSE(bits{const_value} != 2);
	static_assert(!(bits{constexpr_value} != 3));

	EXPECT_FALSE(bits{value} != bits{value});
	EXPECT_FALSE(bits{const_value} != bits{const_value});
	static_assert(!(bits{constexpr_value} != bits{constexpr_value}));

	EXPECT_TRUE(bits{value} != bits{const_value});
	EXPECT_TRUE(bits{const_value} != bits{value});
	EXPECT_TRUE(bits{value} != bits{constexpr_value});
}

TEST_F(operators, greater) {
	EXPECT_TRUE(bits{value} > 0);
	EXPECT_TRUE(bits{const_value} > 1);
	static_assert(bits{constexpr_value} > 2);

	EXPECT_FALSE(bits{value} > bits{value});
	EXPECT_FALSE(bits{const_value} > bits{const_value});
	static_assert(!(bits{constexpr_value} > bits{constexpr_value}));

	EXPECT_TRUE(bits{const_value} > bits{value});
	EXPECT_FALSE(bits{value} > bits{const_value});
	EXPECT_TRUE(bits{constexpr_value} > bits{value});
}

TEST_F(operators, greater_equal) {
	EXPECT_TRUE(bits{value} >= 1);
	EXPECT_TRUE(bits{const_value} >= 2);
	static_assert(bits{constexpr_value} >= 2);

	EXPECT_TRUE(bits{value} >= bits{value});
	EXPECT_TRUE(bits{const_value} >= bits{const_value});
	static_assert(bits{constexpr_value} >= bits{constexpr_value});

	EXPECT_TRUE(bits{const_value} >= bits{value});
	EXPECT_FALSE(bits{value} >= bits{const_value});
	EXPECT_TRUE(bits{constexpr_value} >= bits{value});
}

TEST_F(operators, less) {
	EXPECT_TRUE(bits{value} < 2);
	EXPECT_TRUE(bits{const_value} < 3);
	static_assert(bits{constexpr_value} < 4);

	EXPECT_FALSE(bits{value} < bits{value});
	EXPECT_FALSE(bits{const_value} < bits{const_value});
	static_assert(!(bits{constexpr_value} < bits{constexpr_value}));

	EXPECT_FALSE(bits{const_value} < bits{value});
	EXPECT_TRUE(bits{value} < bits{const_value});
	EXPECT_FALSE(bits{constexpr_value} < bits{value});
}

TEST_F(operators, less_equal) {
	EXPECT_TRUE(bits{value} <= 1);
	EXPECT_TRUE(bits{const_value} <= 2);
	static_assert(bits{constexpr_value} <= 4);

	EXPECT_TRUE(bits{value} <= bits{value});
	EXPECT_TRUE(bits{const_value} <= bits{const_value});
	static_assert(bits{constexpr_value} <= bits{constexpr_value});

	EXPECT_FALSE(bits{const_value} <= bits{value});
	EXPECT_TRUE(bits{value} <= bits{const_value});
	EXPECT_FALSE(bits{constexpr_value} <= bits{value});
}

}
