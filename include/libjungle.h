#ifndef JUNGLE_H
#define JUNGLE_H

#include <functional>
#include <memory>
#include <soundio/soundio.h>
#include <vector>

namespace jungle {

const float SAMPLE_RATE_HZ = 48000.0;

void eventloop();

namespace tempo {

	using Func = std::function<void()>;

	class Tempo {
	public:
		int bpm;
		int period_us;
		Tempo(int bpm);
		void start();
		void register_func_cycle(std::vector<Func> cycle);

	private:
		std::vector<std::vector<Func>> func_cycles;
		std::vector<size_t> func_cycle_indices;
	};
}; // namespace tempo

namespace audio {

	using Tone = std::vector<float>;
	Tone generate_tone(float pitch_hz);

	class Engine {
	public:
		Engine();
		~Engine();

		class Stream {
			friend class Engine;

		public:
			Stream() = delete;
			~Stream();
			void play_tone(Tone& tone);

		private:
			Stream(struct SoundIoDevice* device);
			struct SoundIoOutStream* outstream;
			struct SoundIoRingBuffer* ringbuf;
		};

		Stream new_stream();

	private:
		struct SoundIo* soundio;
		struct SoundIoDevice* device;
	};
}; // namespace audio
}; // namespace jungle

#endif /* JUNGLE_H */
