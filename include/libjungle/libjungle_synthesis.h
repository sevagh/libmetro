#ifndef JUNGLE_SYNTHESIS_H
#define JUNGLE_SYNTHESIS_H

#include "libjungle.h"
#include <atomic>
#include <list>
#include <map>
#include <stk/Stk.h>
#include <vector>
//#include <functional.h>

namespace jungle {
namespace synthesis {
	namespace timbre {
		class Timbre {
		public:
			virtual std::vector<float>& get_frames() = 0;
		};

		void play_on_stream(jungle::core::audio::Engine::OutStream& stream,
		                    std::list<Timbre*> timbres);

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

	namespace metronome {
		jungle::core::event::EventCycle
		metronome_common_time(jungle::core::audio::Engine::OutStream& stream);

		jungle::core::event::EventCycle
		metronome_waltz_time(jungle::core::audio::Engine::OutStream& stream);

		jungle::core::event::EventCycle find_metro_by_time_signature(
		    std::string ts,
		    jungle::core::audio::Engine::OutStream& stream);
	}; // namespace metronome
};     // namespace synthesis
};     // namespace jungle

#endif /* JUNGLE_SYNTHESIS_H */
