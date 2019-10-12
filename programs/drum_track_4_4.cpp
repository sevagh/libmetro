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
	auto stream = audio_engine.new_stream(tempo.get_period_us());

	std::cout << "init audio engine" << std::endl;
	std::cout << "Generating tones" << std::endl;

	auto hihat = jungle::audio::timbre::Drum(42);
	auto snare = jungle::audio::timbre::Drum(38);
	auto bass = jungle::audio::timbre::Drum(45);

	jungle::event::EventCycle beat22 = jungle::event::EventCycle(std::vector<jungle::event::EventFunc>({
	    [&]() {
		    jungle::audio::timbre::play_on_stream(stream, {&hihat, &snare});
	    },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, {&hihat}); },
	    [&]() {
		    jungle::audio::timbre::play_on_stream(stream, {&hihat, &bass});
	    },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, {&hihat}); },
	}));

	tempo.register_event_cycle(beat22);
	tempo.start();

	audio_engine.eventloop();
}
