#include "libjungle.h"
#include <gtest/gtest.h>

TEST(Tempo, HelloWorld)
{
	auto x = "Hello, world!";
	EXPECT_EQ(x, x);

	auto tempo = jungle::tempo::Tempo(200); // 200bpm
	tempo.start();
	tempo.stop();
	tempo.reset();
}
