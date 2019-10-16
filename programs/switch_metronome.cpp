#include "libjungle/libjungle.h"
#include "libjungle/libjungle_synthesis.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 6) {
		std::cerr << "Usage: " << argv[0] << " ts1 measures1 ts2 measures2 bpm"
		          << std::endl;
		exit(1);
	}

	std::string ts1 = argv[1];
	int measures1 = std::stoi(argv[2]);

	std::string ts2 = argv[3];
	int measures2 = std::stoi(argv[4]);

	int bpm = std::stoi(argv[5]);

	auto tempo = jungle::core::tempo::Tempo(bpm);

	auto audio_engine = jungle::core::audio::Engine();
	auto stream1 = audio_engine.new_outstream(tempo.period_us);
	auto stream2 = audio_engine.new_outstream(tempo.period_us);

	std::cout << "init audio engine" << std::endl;

	auto metro1 = jungle::synthesis::metronome::find_metro_by_time_signature(
	    ts1, stream1);
	auto metro2 = jungle::synthesis::metronome::find_metro_by_time_signature(
	    ts2, stream2);

	// start with stream2 muted
	stream2.toggle_mute();

	// schedule to mute/unmute metros
	metro1.schedule_meta_event([&]() { stream1.toggle_mute(); }, measures1);
	metro2.schedule_meta_event([&]() { stream2.toggle_mute(); }, measures2);

	tempo.register_event_cycle(metro1);
	tempo.register_event_cycle(metro2);
	tempo.start();

	audio_engine.eventloop();

	return 0;
}
