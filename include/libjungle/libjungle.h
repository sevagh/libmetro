#ifndef JUNGLE_H
#define JUNGLE_H

#include <atomic>
#include <chrono>
#include <functional>
#include <soundio/soundio.h>
#include <thread>
#include <vector>

namespace jungle {
namespace core {

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
			std::chrono::microseconds period_us;

			Tempo(int bpm);
			~Tempo();

			void start();
			void stop();
			void register_event_cycle(jungle::core::event::EventCycle& cycle);

		private:
			std::vector<jungle::core::event::EventCycle*> event_cycles;
			std::atomic<bool> ticker_on;
			std::thread ticker_thread;
		};
	}; // namespace tempo
	namespace audio {
		class Engine {
		public:
			Engine();
			~Engine();
			void eventloop();

			class OutStream {
				friend class Engine;

			public:
				float latency_s;
				struct SoundIoOutStream* outstream;
				struct SoundIoRingBuffer* ringbuf;
				void set_latency_s(float new_latency_s);
				OutStream() = delete; // disallow the empty constructor
				~OutStream();

			private:
				Engine* parent_engine;
				OutStream(Engine* parent_engine,
				          float latency_s); // private constructor - only
				                            // Engines can create OutStreams
			};

			OutStream new_stream(std::chrono::microseconds ticker_period);

		private:
			struct SoundIo* soundio;
			struct SoundIoDevice* device;
		};
	}; // namespace audio
};     // namespace core
};     // namespace jungle

#endif /* JUNGLE_H */
