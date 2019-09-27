#ifndef JUNGLE_H
#define JUNGLE_H

#include <functional>
#include <memory>
#include <soundio/soundio.h>
#include <stk/Stk.h>
#include <vector>

// re-alias StkFloat to float, not double
typedef float StkFloat;

namespace jungle {

const float SampleRateHz = 48000.0;

using EventFunc = std::function<void()>;

class EventCycle {
public:
	std::vector<EventFunc> events;
	EventCycle(std::vector<EventFunc> events);
	void dispatch_next_event();

private:
	size_t index;
};

namespace tempo {
	class Tempo {
	public:
		int bpm;
		int period_us;
		Tempo(int bpm);
		void start();
		void register_event_cycle(jungle::EventCycle& cycle);

	private:
		std::vector<jungle::EventCycle*> event_cycles;
	};
}; // namespace tempo

namespace audio {
	class Tone;

	class Engine {
	public:
		Engine();
		~Engine();
		void eventloop();

		class Stream {
			friend class Engine;
			friend class Tone;

		public:
			struct SoundIoRingBuffer* ringbuf;
			Stream() = delete;
			~Stream();

		private:
			Engine* parent_engine;
			float latency_s;
			Stream(Engine* parent_engine, float latency_s);
			struct SoundIoOutStream* outstream;
		};

		Stream new_stream(float latency_s);

	private:
		struct SoundIo* soundio;
		struct SoundIoDevice* device;
	};

	class Tone {
	public:
		std::vector<float> tone;
		Tone(float pitch_hz, float volume_pct);
		Tone(float pitch_hz)
		    : Tone(pitch_hz, 100.0){};
		void play_on_stream(Engine::Stream& stream);
	};
}; // namespace audio

namespace metronome {
	// metronome beeps for common time signatures
	extern jungle::audio::Tone StrongDownbeat;
	extern jungle::audio::Tone StrongBeat;
	extern jungle::audio::Tone WeakDownbeat;
	extern jungle::audio::Tone WeakBeat;

	jungle::EventCycle
	metronome_common_time(jungle::audio::Engine::Stream& stream);
}; // namespace metronome
}; // namespace jungle

#endif /* JUNGLE_H */
