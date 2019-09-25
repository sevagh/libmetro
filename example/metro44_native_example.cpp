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

	float best_latency_s = (tempo.period_us / 4.0) / 1000000.0;

	auto audio_engine = jungle::audio::Engine();
	auto stream = audio_engine.new_stream(best_latency_s);

	std::cout << "init audio engine" << std::endl;

	auto metronome_event_cycle
	    = jungle::metronome::metronome_common_time(stream);

	tempo.register_event_cycle(metronome_event_cycle);
	tempo.start();

	audio_engine.eventloop();
}
