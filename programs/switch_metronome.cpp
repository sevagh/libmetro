#include "libjungle/libjungle.h"
#include "libjungle/libjungle_synthesis.h"
#include <iostream>
#include <numeric>
#include <sstream>
#include <tuple>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0]
		          << " ts1,measures1,bpm1 ts2,measures2,bpm2, ..." << std::endl;
		exit(1);
	}

	std::vector<std::string> timesigs(argc - 1);
	std::vector<int> measures(argc - 1);
	std::vector<int> bpms(argc - 1);

	for (int i = 0; i < argc - 1; ++i) {
		std::stringstream ss(argv[i + 1]);

		int j = 0;
		while (ss.good()) {
			std::string substr;
			getline(ss, substr, ',');
			switch (j) {
			case 0:
				timesigs[i] = std::string(substr);
				break;
			case 1:
				measures[i] = std::stoi(substr);
				break;
			case 2:
				bpms[i] = std::stoi(substr);
				break;
			default:
				std::cerr << "must use format time_signature,measures,bpm"
				          << std::endl;
				exit(1);
			}
			j++;
		}
	}

	int common_bpm
	    = std::accumulate(bpms.begin(), bpms.end(), 1, std::lcm<int, int>);
	auto tempo = jungle::core::tempo::Tempo(common_bpm);

	auto audio_engine = jungle::core::audio::Engine();

	std::vector<jungle::core::audio::Engine::OutStream> streams;
	std::vector<jungle::core::event::EventCycle> metro_cycles;

	for (size_t i = 0; i < timesigs.size(); ++i) {
		auto stream = audio_engine.new_outstream(tempo.period_us);
		streams.push_back(stream);
		metro_cycles.push_back(
		    jungle::synthesis::metronome::find_metro_by_time_signature(
		        timesigs[i], streams[i]));
	}

	std::vector<int> quarter_notes_per_measure(argc - 1);
	std::vector<int> total_metro_qnotes(argc - 1);
	for (size_t i = 0; i < measures.size(); ++i) {
		quarter_notes_per_measure[i] = metro_cycles[i].events.size();
		total_metro_qnotes[i] = quarter_notes_per_measure[i] * measures[i];
	}

	// an empty timbre to keep outputting data to the audio device
	auto empty = jungle::synthesis::timbre::Empty();

	// a large vector to encompass both two time signatures * elapsed measures
	int total_quarter_notes = std::accumulate(
	    total_metro_qnotes.begin(), total_metro_qnotes.end(), 0);

	std::vector<jungle::core::event::EventFunc> keep_streams_warm(
	    total_quarter_notes);

	// at every tick, emit a "blank" timbre to the ringbuffers to keep the
	// audio device "spinning" this is _tremendously_ ugly but i had no luck
	// with pausing the streams that should be silenced if the ringbuffer is
	// not filled at every tick, all my assumptions based on soundio's latency
	// are nullified
	for (size_t i = 0; i < keep_streams_warm.size(); ++i) {
		keep_streams_warm[i] = [&]() {
			for (auto stream : streams)
				jungle::synthesis::timbre::play_on_stream(stream, {&empty});
		};
	}

	std::vector<jungle::core::event::EventFunc> metro_vec(total_quarter_notes);

	size_t metro_vec_idx = 0;
	for (auto metro_cycle : metro_cycles)
		for (auto metro_tick : metro_cycle.events)
			metro_vec[metro_vec_idx++] = metro_tick;

	auto metro = jungle::core::event::EventCycle(metro_vec);
	auto keep_warm = jungle::core::event::EventCycle(keep_streams_warm);

	tempo.register_event_cycle(metro);
	tempo.register_event_cycle(keep_warm);
	tempo.start();

	audio_engine.eventloop();

	return 0;
}
