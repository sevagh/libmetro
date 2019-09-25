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
	auto stream = audio_engine.new_stream();

	std::cout << "init audio engine" << std::endl;
	std::cout << "Generating tones" << std::endl;

	auto strong_downbeat = jungle::audio::Tone(440.0, 100.0);
	auto strong_beat = jungle::audio::Tone(350.0, 100.0);

	auto weak_downbeat = jungle::audio::Tone(440.0, 50.0);
	auto weak_beat = jungle::audio::Tone(350.0, 50.0);

	// create a cycle of lambdas
	jungle::EventCycle beat22
	    = jungle::EventCycle(std::vector<jungle::EventFunc>({
	        [&]() { strong_downbeat.play_on_stream(stream); },
	        [&]() { strong_beat.play_on_stream(stream); },
	        [&]() { weak_downbeat.play_on_stream(stream); },
	        [&]() { weak_beat.play_on_stream(stream); },
	    }));

	tempo.register_event_cycle(beat22);
	tempo.start();

	audio_engine.eventloop();
}
