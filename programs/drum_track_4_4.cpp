#include "libjungle/libjungle.h"
#include "libjungle/libjungle_synthesis.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " bpm" << std::endl;
		exit(1);
	}

	int bpm = std::stoi(argv[1]);
	auto tempo = jungle::core::tempo::Tempo(bpm);

	std::cout << "init " << bpm << "bpm tempo ticker" << std::endl;

	auto audio_engine = jungle::core::audio::Engine();
	auto stream = audio_engine.new_outstream(tempo.period_us);

	std::cout << "init audio engine" << std::endl;
	std::cout << "Generating tones" << std::endl;

	auto hihat = jungle::synthesis::timbre::Drum(42, 100);
	auto snare = jungle::synthesis::timbre::Drum(38, 100);
	auto bass = jungle::synthesis::timbre::Drum(45, 100);

	jungle::core::event::EventCycle beat22 = jungle::core::event::EventCycle(std::vector<
	                                                                         jungle::core::event::
	                                                                             EventFunc>({
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(stream, {&hihat, &snare});
	    },
	    [&]() { jungle::synthesis::timbre::play_on_stream(stream, {&hihat}); },
	    [&]() {
		    jungle::synthesis::timbre::play_on_stream(stream, {&hihat, &bass});
	    },
	    [&]() { jungle::synthesis::timbre::play_on_stream(stream, {&hihat}); },
	}));

	tempo.register_event_cycle(beat22);
	tempo.start();

	audio_engine.eventloop();
}
