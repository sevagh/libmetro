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
	int bpm;
	Tempo(int bpm);
	~Tempo();

	void start();
	void stop();
	void add_measure(metro::NoteLength note_length, metro::Measure& measure);

private:
	AudioEngine engine;
	AudioEngine::OutStream& streams[4];

	std::atomic<bool> ticker_on;
	std::thread ticker_threads[4];
	std::chrono::microseconds period_us[4];
};
}; // namespace metro_private

#endif /* TIMING_H */
