#include "libmetro.h"
#include <gtest/gtest.h>

TEST(AudioEngine, InitNoMemoryLeaks)
{
	auto audio_engine = metro::audio::Engine();
}
