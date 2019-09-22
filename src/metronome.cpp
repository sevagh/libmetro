#include "libjungle.h"

jungle::audio::Tone jungle::metronome::StrongDownbeat
    = jungle::audio::generate_tone(440.0, 100.0);
jungle::audio::Tone jungle::metronome::StrongBeat
    = jungle::audio::generate_tone(350.0, 100.0);
jungle::audio::Tone jungle::metronome::WeakDownbeat
    = jungle::audio::generate_tone(440.0, 50.0);
jungle::audio::Tone jungle::metronome::WeakBeat
    = jungle::audio::generate_tone(350.0, 50.0);

jungle::EventCycle
jungle::metronome::metronome_common_time(jungle::audio::Engine::Stream& stream)
{
	return jungle::EventCycle({
	    [&]() { stream.play_tone(jungle::metronome::StrongDownbeat); },
	    [&]() { stream.play_tone(jungle::metronome::StrongBeat); },
	    [&]() { stream.play_tone(jungle::metronome::WeakDownbeat); },
	    [&]() { stream.play_tone(jungle::metronome::WeakBeat); },
	});
}
