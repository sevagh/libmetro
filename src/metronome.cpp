#include "libjungle.h"

static jungle::audio::Tone strong_downbeat = jungle::audio::Tone(540.0, 100.0);
static jungle::audio::Tone strong_beat = jungle::audio::Tone(350.0, 100.0);
static jungle::audio::Tone weak_downbeat = jungle::audio::Tone(440.0, 50.0);
static jungle::audio::Tone weak_beat = jungle::audio::Tone(350.0, 50.0);

jungle::EventCycle
jungle::metronome::metronome_common_time(jungle::audio::Engine::Stream& stream)
{

	return jungle::EventCycle({
	    [&]() { strong_downbeat.play_on_stream(stream); },
	    [&]() { strong_beat.play_on_stream(stream); },
	    [&]() { weak_downbeat.play_on_stream(stream); },
	    [&]() { weak_beat.play_on_stream(stream); },
	});
}
