#include "libjungle/libjungle.h"
#include "libjungle/libjungle_synthesis.h"
#include <iostream>
#include <vector>

static auto strong_downbeat = jungle::synthesis::timbre::Sine(540.0, 100.0);
static auto strong_beat = jungle::synthesis::timbre::Sine(350.0, 100.0);
static auto weak_downbeat = jungle::synthesis::timbre::Sine(440.0, 50.0);
static auto weak_beat = jungle::synthesis::timbre::Sine(350.0, 50.0);

jungle::core::event::EventCycle
jungle::synthesis::metronome::metronome_common_time(
    jungle::core::audio::Engine::OutStream& stream)
{

	return jungle::core::event::EventCycle({
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(
		        stream, {&strong_downbeat});
		    std::cout << "4/4: 1" << std::endl;
	    },
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(stream, {&strong_beat});
		    std::cout << "4/4: 2" << std::endl;
	    },
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(stream, {&weak_downbeat});
		    std::cout << "4/4: 3" << std::endl;
	    },
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(stream, {&weak_beat});
		    std::cout << "4/4: 4" << std::endl;
	    },
	});
}

jungle::core::event::EventCycle
jungle::synthesis::metronome::metronome_waltz_time(
    jungle::core::audio::Engine::OutStream& stream)
{

	return jungle::core::event::EventCycle({
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(
		        stream, {&strong_downbeat});
		    std::cout << "3/4: 1" << std::endl;
	    },
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(stream, {&weak_beat});
		    std::cout << "3/4: 2" << std::endl;
	    },
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(stream, {&weak_beat});
		    std::cout << "3/4: 3" << std::endl;
	    },
	});
}

jungle::core::event::EventCycle
jungle::synthesis::metronome::find_metro_by_time_signature(
    std::string ts,
    jungle::core::audio::Engine::OutStream& stream)
{
	if (ts.compare("3/4") == 0)
		return jungle::synthesis::metronome::metronome_waltz_time(stream);
	if (ts.compare("4/4") == 0)
		return jungle::synthesis::metronome::metronome_common_time(stream);
	throw std::runtime_error("unsupported time signature");
}
