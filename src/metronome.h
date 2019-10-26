#ifndef LIBMETRO_PRIVATE_H
#define LIBMETRO_PRIVATE_H

#include "audioengine.h"
#include "libmetro.h"
#include "outstream.h"
#include <atomic>
#include <chrono>
#include <soundio/soundio.h>
#include <thread>

#ifdef UNIT_TESTS
#include <gtest/gtest_prod.h>
#endif /* UNIT_TESTS */

namespace metro_private {

class MetronomePrivate {
public:
	MetronomePrivate(int bpm);
	~MetronomePrivate();

	void start();
	void loop();
	void stop();
	void add_measure(metro::NoteLength note_length, metro::Measure& measure);

private:
#ifdef UNIT_TESTS
	// FRIEND_TEST(SoundIoUnitTest, AudioEngineOutputDevice);
	// FRIEND_TEST(SoundIoUnitTest, AudioEngineOutputDualChannel);
#endif /* UNIT_TESTS */
	int bpm;
	AudioEngine engine;

	std::chrono::microseconds period_us_2;
	std::chrono::microseconds period_us_4;
	std::chrono::microseconds period_us_8;
	std::chrono::microseconds period_us_16;

	OutStream stream_2;
	OutStream stream_4;
	OutStream stream_8;
	OutStream stream_16;

	std::atomic<bool> tickers_on;

	std::thread ticker_thread_2;
	std::thread ticker_thread_4;
	std::thread ticker_thread_8;
	std::thread ticker_thread_16;
};
}; // namespace metro_private

#endif /* LIBMETRO_PRIVATE_H */
