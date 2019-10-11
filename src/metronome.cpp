#include "libjungle.h"
#include <vector>

static auto strong_downbeat = jungle::audio::timbre::Pulse(540.0, 100.0);
static auto strong_beat = jungle::audio::timbre::Pulse(350.0, 100.0);
static auto weak_downbeat = jungle::audio::timbre::Pulse(440.0, 50.0);
static auto weak_beat = jungle::audio::timbre::Pulse(350.0, 50.0);

jungle::EventCycle
jungle::metronome::metronome_common_time(jungle::audio::Engine::Stream& stream)
{

	return jungle::EventCycle({
	    [&]() {
		    jungle::audio::timbre::play_on_stream(stream, {&strong_downbeat});
	    },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, {&strong_beat}); },
	    [&]() {
		    jungle::audio::timbre::play_on_stream(stream, {&weak_downbeat});
	    },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, {&weak_beat}); },
	});
}
