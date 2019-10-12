#include "libjungle.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " bpm" << std::endl;
		exit(1);
	}

	int bpm = std::stoi(argv[1]);
	auto tempo = jungle::tempo::Tempo(bpm);

	std::cout << "init " << bpm << "bpm tempo ticker" << std::endl;

	auto audio_engine = jungle::audio::Engine();
	auto stream = audio_engine.new_stream(tempo.period_us);

	std::cout << "init audio engine" << std::endl;

	auto strong_downbeat = jungle::audio::timbre::Pulse(540.0, 100.0);
	auto strong_beat = jungle::audio::timbre::Pulse(350.0, 100.0);
	auto weak_downbeat = jungle::audio::timbre::Pulse(440.0, 50.0);
	auto weak_beat = jungle::audio::timbre::Pulse(350.0, 50.0);

	auto metronome_event_cycle
		= jungle::event::EventCycle({
	    [&]() {
		    jungle::audio::timbre::play_on_stream(stream, {&strong_downbeat});
	    },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, {&strong_beat}); },
	    [&]() {
		    jungle::audio::timbre::play_on_stream(stream, {&weak_downbeat});
	    },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, {&weak_beat}); },
	});

	tempo.register_event_cycle(metronome_event_cycle);
	tempo.start();

	audio_engine.eventloop();
}
