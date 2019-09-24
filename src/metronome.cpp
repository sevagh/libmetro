#include "libjungle.h"
#include <iostream>

jungle::audio::Tone jungle::metronome::StrongDownbeat
    = jungle::audio::Tone(440.0, 100.0);
jungle::audio::Tone jungle::metronome::StrongBeat
    = jungle::audio::Tone(350.0, 100.0);
jungle::audio::Tone jungle::metronome::WeakDownbeat
    = jungle::audio::Tone(440.0, 50.0);
jungle::audio::Tone jungle::metronome::WeakBeat
    = jungle::audio::Tone(350.0, 50.0);

jungle::EventCycle
jungle::metronome::metronome_common_time(jungle::audio::Engine::Stream& stream,
                                         int duration_us)
{
	std::cout << "DURATION US IN METRO: " << duration_us << std::endl;
	return jungle::EventCycle({
	    [&, duration_us]() {
		    jungle::metronome::StrongDownbeat.play_on_stream(
		        stream, duration_us);
	    },
	    [&, duration_us]() {
		    jungle::metronome::StrongBeat.play_on_stream(stream, duration_us);
	    },
	    [&, duration_us]() {
		    jungle::metronome::WeakDownbeat.play_on_stream(stream, duration_us);
	    },
	    [&, duration_us]() {
		    jungle::metronome::WeakBeat.play_on_stream(stream, duration_us);
	    },
	});
}
