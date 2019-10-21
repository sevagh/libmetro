#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "libmetro.h"
#include <soundio/soundio.h>
#include <chrono>

namespace metro_private {
class AudioEngine {
friend class OutStream;

public:
	AudioEngine();
	~AudioEngine();

	class OutStream {
		friend class AudioEngine;

	public:
		OutStream() = delete; // disallow the empty constructor
		~OutStream();

		void add_measure(metro::Measure& measure);
		void play_next_note();

	private:
		OutStream(AudioEngine* parent_engine, float latency_s); // private constructor - only engines can build streams

		float latency_s;
		struct SoundIoRingBuffer* ringbuf;
		struct SoundIoOutStream* outstream;
		std::vector<metro::Measure&> measures;
		std::vector<size_t> measure_indices;
	};

	OutStream new_outstream(std::chrono::microseconds ticker_period);

private:
	struct SoundIo* soundio;
	struct SoundIoDevice* device;
};


}; // namespace metro_private

#endif /* AUDIOENGINE_H */
