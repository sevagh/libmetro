#ifndef TIMING_H
#define TIMING_H

#include "libmetro.h"
#include "audioengine.h"
#include <chrono>
#include <thread>
#include <atomic>

namespace metro_private {
class Tempo {
public:
	Tempo(int bpm);
	~Tempo();

	void start();
	void stop();
	void add_measure(metro::NoteLength note_length, metro::Measure& measure);

private:
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

#endif /* TIMING_H */
