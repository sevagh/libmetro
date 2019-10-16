#include "libjungle/libjungle.h"
#include "libjungle/libjungle_synthesis.h"
#include <iostream>
#include <numeric>

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

	auto metro1 = jungle::synthesis::metronome::find_metro_by_time_signature(
	    ts1, stream1);
	auto metro2 = jungle::synthesis::metronome::find_metro_by_time_signature(
	    ts2, stream2);

	size_t quarter_notes_per_measure_1 = metro1.events.size();
	size_t quarter_notes_per_measure_2 = metro2.events.size();

	int total_metro1 = measures1 * quarter_notes_per_measure_1;
	int total_metro2 = measures2 * quarter_notes_per_measure_2;

	// a large vector to encompass both two time signatures * elapsed measures
	int total_quarter_notes = total_metro1 + total_metro2;

	auto empty = jungle::synthesis::timbre::Empty();

	std::vector<jungle::core::event::EventFunc> metro_vec(total_quarter_notes);
	std::vector<jungle::core::event::EventFunc> keep_stream_warm(
	    total_quarter_notes);

	for (size_t i = 0; i < metro_vec.size(); ++i) {
		if (i < ( size_t )total_metro1) {
			metro_vec[i] = metro1.events[i % quarter_notes_per_measure_1];
			keep_stream_warm[i] = [&]() {
				jungle::synthesis::timbre::play_on_stream(stream2, {&empty});
			};
		}
		else {
			metro_vec[i] = metro2.events[(i - ( size_t )total_metro1)
			                             % quarter_notes_per_measure_2];
			keep_stream_warm[i] = [&]() {
				jungle::synthesis::timbre::play_on_stream(stream1, {&empty});
			};
		}
	}

	auto metro = jungle::core::event::EventCycle(metro_vec);
	auto keep_warm = jungle::core::event::EventCycle(keep_stream_warm);

	tempo.register_event_cycle(metro);
	tempo.register_event_cycle(keep_warm);
	tempo.start();

	audio_engine.eventloop();

	return 0;
}
