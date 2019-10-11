#include "libjungle.h"
#include <gtest/gtest.h>

TEST(AudioEngine, InitNoMemoryLeaks)
{
	auto audio_engine = jungle::audio::Engine();
}
