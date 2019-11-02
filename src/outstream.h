#ifndef OUTSTREAM_H
#define OUTSTREAM_H

#include "libmetro.h"
#include <chrono>
#include <soundio/soundio.h>

#ifdef UNIT_TESTS
#include <gtest/gtest_prod.h>
#endif /* UNIT_TESTS */

namespace metro_private {
class AudioEngine;

class OutStream {
	friend class AudioEngine;

public:
	OutStream() = delete; // disallow the empty constructor
	~OutStream();
	OutStream(const OutStream& other) = delete;      // disable copy
	OutStream& operator=(const OutStream&) = delete; // disable move

	void add_measure(metro::Measure& measure);
	void play_next_note();
	void start();
	bool has_measures();

private:
#ifdef UNIT_TESTS
	FRIEND_TEST(SoundIoUnitTest, OutStreamCorrectLatency);
	FRIEND_TEST(SoundIoUnitTest, OutStreamCorrectSampleRate);
	FRIEND_TEST(SoundIoUnitTest, OutStreamCorrectRingbufferCapacity);
	FRIEND_TEST(MetronomePrivateUnitTest, AddMismatchedMeasuresLCMSize);
	FRIEND_TEST(MetronomePrivateUnitTest,
	            DISABLED_TestStreamsDontUnderflowOrError);
#endif /* UNIT_TESTS */
	float latency_s;
	AudioEngine* parent_engine;

	struct SoundIoRingBuffer* ringbuf;
	struct SoundIoOutStream* outstream;
	std::vector<metro::Measure> measures;
	std::vector<size_t> measure_indices;

	std::vector<metro::Note> notes;
	size_t note_index;

	OutStream(AudioEngine* parent_engine,
	          std::chrono::microseconds ticker_period); // private constructor
	                                                    // - only engines can
	                                                    // build streams
	void compute_notes();
};
}; // namespace metro_private

#endif /* OUTSTREAM_H */
