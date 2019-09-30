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
	float best_latency_s = (tempo.period_us / 2.0) / 1000000.0;
	std::cout << "best latency: " << best_latency_s << std::endl;

	auto audio_engine = jungle::audio::Engine();
	auto stream = audio_engine.new_stream(best_latency_s);

	std::cout << "init audio engine" << std::endl;
	std::cout << "Generating tones" << std::endl;

	auto strong_downbeat = jungle::audio::timbre::DrumTap(100.0);
	auto strong_beat = jungle::audio::timbre::DrumTap(100.0);
	auto weak_downbeat = jungle::audio::timbre::DrumTap(50.0);
	auto weak_beat = jungle::audio::timbre::DrumTap(50.0);

	// create a cycle of lambdas
	jungle::EventCycle beat22 = jungle::EventCycle(std::vector<jungle::EventFunc>({
	    [&]() {
		    jungle::audio::timbre::play_on_stream(stream, strong_downbeat);
	    },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, strong_beat); },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, weak_downbeat); },
	    [&]() { jungle::audio::timbre::play_on_stream(stream, weak_beat); },
	}));

	tempo.register_event_cycle(beat22);
	tempo.start();

	audio_engine.eventloop();
}
