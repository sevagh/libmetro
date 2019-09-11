#include "rhythm_n_beats.h"
#include <gtest/gtest.h>

TEST(Rnb, HelloWorld)
{
	auto x = rnb::hello_world();
	EXPECT_EQ(x, std::string("Hello, world!\n"));
}
