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

		class Pulse : public Timbre {
		public:
			Pulse(float pitch_hz, float volume_pct);
			Pulse(float pitch_hz)
			    : Pulse(pitch_hz, 100.0){};
			std::vector<float>& get_frames() { return frames; }

		private:
			std::vector<float> frames;
		};

		class Drum : public Timbre {
		public:
			Drum(int midi_drum_instrument, float volume_pct);
			Drum(int midi_drum_instrument)
			    : Drum(midi_drum_instrument, 100.0){};
			std::vector<float>& get_frames() { return frames; }

		private:
			std::vector<float> frames;
		};
	}; // namespace timbre

	namespace metronome {
		jungle::core::event::EventCycle
		metronome_common_time(jungle::core::audio::Engine::OutStream& stream);

		static std::map<std::string,
		                std::function<jungle::core::event::EventCycle(
		                    jungle::core::audio::Engine::OutStream&)>>
		    time_signature_mappings = {
		        {"4/4", metronome_common_time},
		};
	}; // namespace metronome

}; // namespace synthesis
}; // namespace jungle

#endif /* JUNGLE_SYNTHESIS_H */
