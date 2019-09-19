#include "libjungle.h"
#include <iostream>

int main()
{
	int bpm = 10;
	auto tempo = jungle::tempo::Tempo(bpm);

	std::cout << "init " << bpm << "bpm tempo ticker" << std::endl;

	auto audio_engine = jungle::audio::Engine();
	auto stream = jungle::audio::Stream(audio_engine, tempo.period_us / 10);

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
