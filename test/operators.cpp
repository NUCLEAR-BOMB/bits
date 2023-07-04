#include <gtest/gtest.h>
#include <bits.hpp>

namespace {

struct operators : ::testing::Test {
	int value = 1;
};

TEST_F(operators, assigment) {
	bits{value} = 2;
	EXPECT_EQ(value, 2);
}

}
