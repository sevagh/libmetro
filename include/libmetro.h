#ifndef LIBMETRO_H
#define LIBMETRO_H

#include <atomic>
#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <soundio/soundio.h>
#include <stk/Stk.h>
#include <thread>
#include <vector>

namespace metro {
const float SampleRateHz = 48000.0;

using QuarterNote = std::function<void()>;

class Measure {
public:
	std::vector<QuarterNote> quarter_notes;

	Measure(std::vector<QuarterNote> quarter_notes)
	    : quarter_notes(quarter_notes)
	    , index(0){};

	void play_next_quarter_note()
	{
		auto ret = quarter_notes.at(index);
		index = (index + 1) % quarter_notes.size();
		std::thread(ret).detach();
	};

private:
	size_t index; // current quarter note
};

namespace timbre {
	class Timbre {
	public:
		virtual std::vector<float>& get_frames() = 0;
	};

	class Empty : public Timbre {
	public:
		Empty()
		    : frames(std::vector<float>(2 * metro::SampleRateHz)){};
		std::vector<float>& get_frames() { return frames; }

	private:
		std::vector<float> frames;
	};

	class Sine : public Timbre {
	public:
		Sine(float pitch_hz, float volume_pct);
		std::vector<float>& get_frames() { return frames; }

	private:
		std::vector<float> frames;
	};

	class Drum : public Timbre {
	public:
		Drum(int midi_drum_instrument, float volume_pct);
		std::vector<float>& get_frames() { return frames; }

	private:
		std::vector<float> frames;
	};
}; // namespace timbre

// a steady-clock based precise sleep with 1ns precision
// or as close to 1ns as nanosleep gets us on a GPOS
void precise_sleep_us(std::chrono::microseconds dur_us);

class Tempo {
public:
	int bpm;
	std::chrono::microseconds period_us;

	Tempo(int bpm);
	~Tempo();

	void start();
	void stop();
	void register_measure(Measure& measure);

private:
	std::vector<Measure*> measures;
	std::atomic<bool> ticker_on;
	std::thread ticker_thread;
};

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

			struct SoundIoRingBuffer* ringbuf;
			struct SoundIoOutStream* outstream;
			OutStream() = delete; // disallow the empty constructor
			~OutStream();
			void play_timbres(std::list<timbre::Timbre*> timbres);

		private:
			Engine* parent_engine;
			OutStream(Engine* parent_engine,
			          float latency_s); // private constructor - only
			                            // Engines can create OutStreams
		};

		OutStream new_outstream(std::chrono::microseconds ticker_period);

	private:
		struct SoundIo* soundio;
		struct SoundIoDevice* device;
	};
}; // namespace audio
}; // namespace metro

#endif /* LIBMETRO_H */
