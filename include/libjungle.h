#ifndef JUNGLE_H
#define JUNGLE_H

#include <atomic>
#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <soundio/soundio.h>
#include <stk/Stk.h>
#include <thread>
#include <vector>

namespace jungle {

const float SampleRateHz = 48000.0;

namespace event {
	using EventFunc = std::function<void()>;

	class EventCycle {
	public:
		std::vector<EventFunc> events;
		EventCycle(std::vector<EventFunc> events);
		void dispatch_next_event();

	private:
		size_t index;
	};
}; // namespace event

namespace tempo {
	// a steady-clock based precise sleep with 1ns precision
	// or as close to 1ns as nanosleep gets us on a GPOS
	void precise_sleep_us(std::chrono::microseconds dur_us);

	// mini helper functions to convert bpm to period and vice versa
	std::chrono::microseconds bpm_to_us(int bpm);
	int us_to_bpm(std::chrono::microseconds us);

	class Tempo {
	public:
		int bpm;

		Tempo(int bpm);
		~Tempo();

		std::chrono::microseconds get_period_us();
		void set_bpm(int new_bpm);
		void start();
		void stop();
		void register_event_cycle(jungle::event::EventCycle& cycle);

	private:
		std::vector<jungle::event::EventCycle*> event_cycles;
		std::atomic<std::chrono::microseconds> period_us;
		std::atomic<bool> ticker_on;
		std::thread ticker_thread;
	};
}; // namespace tempo

namespace audio {
	// chooses an optimal Soundio output stream latency for the
	// target bpm
	float pick_best_latency(std::chrono::microseconds ticker_period);

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
			void set_latency_s(float new_latency_s);
			Stream() = delete; // disallow the empty constructor
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

		void play_on_stream(Engine::Stream& stream, std::list<Timbre*> timbres);

		class Pulse : public Timbre {
		public:
			Pulse(float pitch_hz, float volume_pct);
			Pulse(float pitch_hz)
			    : Pulse(pitch_hz, 100.0){};
			stk::StkFrames& get_frames() { return frames; }

		private:
			stk::StkFrames frames;
		};

		class Drum : public Timbre {
		public:
			Drum(int midi_drum_instrument, float volume_pct);
			Drum(int midi_drum_instrument)
			    : Drum(midi_drum_instrument, 100.0){};
			stk::StkFrames& get_frames() { return frames; }

		private:
			stk::StkFrames frames;
		};
	}; // namespace timbre
};     // namespace audio
};     // namespace jungle

#endif /* JUNGLE_H */
