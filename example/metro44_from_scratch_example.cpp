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

	auto downbeat = jungle::audio::generate_tone(440.0);
	auto beat = jungle::audio::generate_tone(350.0);

	// create a cycle of lambdas
	std::vector<jungle::tempo::Func> beat22 = {
	    [&]() { stream.play_tone(downbeat); },
	    [&]() { stream.play_tone(beat); },
	    [&]() { stream.play_tone(beat); },
	    [&]() { stream.play_tone(beat); },
	};

	tempo.register_func_cycle(beat22);
	tempo.start();

	jungle::eventloop();
}
