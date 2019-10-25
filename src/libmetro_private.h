#ifndef LIBMETRO_PRIVATE_H
#define LIBMETRO_PRIVATE_H

#include "libmetro.h"
#include <atomic>
#include <chrono>
#include <soundio/soundio.h>
#include <thread>

#ifdef UNIT_TESTS
#include <gtest/gtest_prod.h>
#endif /* UNIT_TESTS */

namespace metro_private {
class AudioEngine {
	friend class OutStream;

public:
	AudioEngine();
	~AudioEngine();
	AudioEngine(const AudioEngine& other) = delete;      // disable copy
	AudioEngine& operator=(const AudioEngine&) = delete; // disable move

	void eventloop();

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
#endif /* UNIT_TESTS */
		float latency_s;
		AudioEngine* parent_engine;

		struct SoundIoRingBuffer* ringbuf;
		struct SoundIoOutStream* outstream;
		std::vector<metro::Measure> measures;
		std::vector<size_t> measure_indices;

		OutStream(AudioEngine* parent_engine,
		          float latency_s); // private constructor - only engines can
		                            // build streams
	};

	OutStream new_outstream(std::chrono::microseconds ticker_period);

private:
#ifdef UNIT_TESTS
	FRIEND_TEST(SoundIoUnitTest, AudioEngineOutputDevice);
	FRIEND_TEST(SoundIoUnitTest, AudioEngineOutputDualChannel);
#endif /* UNIT_TESTS */
	struct SoundIo* soundio;
	struct SoundIoDevice* device;
};

class MetronomePrivate {
public:
	MetronomePrivate(int bpm);
	~MetronomePrivate();

	void start();
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

	AudioEngine::OutStream stream_2;
	AudioEngine::OutStream stream_4;
	AudioEngine::OutStream stream_8;
	AudioEngine::OutStream stream_16;

	std::atomic<bool> tickers_on;

	std::thread ticker_thread_2;
	std::thread ticker_thread_4;
	std::thread ticker_thread_8;
	std::thread ticker_thread_16;
};
}; // namespace metro_private

#endif /* LIBMETRO_PRIVATE_H */
