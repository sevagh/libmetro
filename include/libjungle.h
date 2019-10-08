#ifndef JUNGLE_H
#define JUNGLE_H

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <soundio/soundio.h>
#include <stk/Stk.h>
#include <thread>
#include <vector>

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
		std::chrono::microseconds period_us;
		Tempo(int bpm);
		~Tempo();
		void start();
		void stop();
		void register_event_cycle(jungle::EventCycle& cycle);

	private:
		std::vector<jungle::EventCycle*> event_cycles;
		std::thread ticker;
	};
}; // namespace tempo

namespace audio {
	class Engine {
	public:
		Engine();
		~Engine();
		void eventloop();

		class Stream {
			friend class Engine;

		public:
			float latency_s;
			struct SoundIoOutStream* outstream;
			struct SoundIoRingBuffer* ringbuf;
			Stream() = delete;
			~Stream();

		private:
			Engine* parent_engine;
			Stream(Engine* parent_engine,
			       float latency_s); // private constructor - only Engines can
			                         // create Streams
		};

		Stream new_stream(std::chrono::microseconds ticker_period);

	private:
		struct SoundIo* soundio;
		struct SoundIoDevice* device;
	};

	namespace timbre {
		class Timbre {
		public:
			virtual stk::StkFrames& get_frames() = 0;
		};

		void play_on_stream(Engine::Stream& stream, Timbre& timbre);

		class Pulse : public Timbre {
		public:
			Pulse(float pitch_hz, float volume_pct);
			Pulse(float pitch_hz)
			    : Pulse(pitch_hz, 100.0){};
			stk::StkFrames& get_frames() { return frames; }

		private:
			stk::StkFrames frames;
		};

		class DrumTap : public Timbre {
		public:
			DrumTap(float volume_pct);
			DrumTap()
			    : DrumTap(100.0){};
			stk::StkFrames& get_frames() { return frames; }

		private:
			stk::StkFrames frames;
		};
	}; // namespace timbre
};     // namespace audio

namespace metronome {
	jungle::EventCycle
	metronome_common_time(jungle::audio::Engine::Stream& stream);

	static std::map<
	    std::string,
	    std::function<jungle::EventCycle(jungle::audio::Engine::Stream&)>>
	    time_signature_mappings = {
	        {"4/4", metronome_common_time},
	};
}; // namespace metronome
}; // namespace jungle

#endif /* JUNGLE_H */
