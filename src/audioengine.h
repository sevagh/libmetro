#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "libmetro.h"
#include <chrono>
#include <soundio/soundio.h>

#ifdef UNIT_TESTS
#include <gtest/gtest_prod.h>
#endif /* UNIT_TESTS */

namespace metro_private {
class OutStream;

class AudioEngine {
	friend class OutStream;

public:
	AudioEngine();
	~AudioEngine();
	AudioEngine(const AudioEngine& other) = delete;      // disable copy
	AudioEngine& operator=(const AudioEngine&) = delete; // disable move

	OutStream new_outstream(std::chrono::microseconds ticker_period);
	void eventloop();

private:
#ifdef UNIT_TESTS
	FRIEND_TEST(SoundIoUnitTest, AudioEngineOutputDevice);
	FRIEND_TEST(SoundIoUnitTest, AudioEngineOutputDualChannel);
#endif /* UNIT_TESTS */
	struct SoundIo* soundio;
	struct SoundIoDevice* device;
};
}; // namespace metro_private

#endif /* AUDIOENGINE_H */
