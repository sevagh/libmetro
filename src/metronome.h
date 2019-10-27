#ifndef LIBMETRO_PRIVATE_H
#define LIBMETRO_PRIVATE_H

#include "audioengine.h"
#include "libmetro.h"
#include <atomic>
#include <chrono>
#include <map>
#include <soundio/soundio.h>
#include <thread>

#ifdef UNIT_TESTS
#include <gtest/gtest_prod.h>
#endif /* UNIT_TESTS */

namespace metro_private {

class OutStream;

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
	FRIEND_TEST(MetronomePrivateUnitTest, EmptyAtInit);
	FRIEND_TEST(MetronomePrivateUnitTest, AddMismatchedMeasuresLCMSize);
	FRIEND_TEST(MetronomePrivateUnitTest, AddDifferentNoteLengthMeasures);
	FRIEND_TEST(MetronomePrivateUnitTest,
	            DISABLED_TestStreamsDontUnderflowOrError);
#endif /* UNIT_TESTS */
	int bpm;
	AudioEngine engine;

	struct NoteTicker {
		metro::NoteLength note_length;
		std::chrono::microseconds period_us;
		OutStream* stream;
		std::thread* ticker_thread;
	};

	std::map<metro::NoteLength, NoteTicker> tickers;
	std::atomic<bool> tickers_on;
};
}; // namespace metro_private

#endif /* LIBMETRO_PRIVATE_H */
