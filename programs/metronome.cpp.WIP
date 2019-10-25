#include "libmetro.h"
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

	metro::timbre::Sine strong_downbeat(540.0, 100.0);
	metro::timbre::Sine strong_beat(350.0, 100.0);
	metro::timbre::Sine weak_downbeat(440.0, 50.0);
	metro::timbre::Sine weak_beat(350.0, 50.0);

	int common_bpm
	    = std::accumulate(bpms.begin(), bpms.end(), 1, std::lcm<int, int>);

	if (common_bpm > 400) {
		std::cerr << "The least common multiple of the input bpms is "
		          << common_bpm
		          << " - for practical concerns we can only go as high as 400"
		          << std::endl;
		exit(1);
	}

	auto tempo = metro::Tempo(common_bpm);
	auto stream = metro::OutStream(tempo.period_us);

	std::vector<int> quarter_notes_per_measure(argc - 1);

	for (size_t i = 0; i < timesigs.size(); ++i) {
		auto bpm_stretch = common_bpm / bpms[i];

		if (timesigs[i].compare("4/4") == 0)
			quarter_notes_per_measure[i] = 4;
		else if (timesigs[i].compare("3/4") == 0)
			quarter_notes_per_measure[i] = 3;
		else
			throw std::runtime_error("unsupported time signature" + timesigs[i]);

		quarter_notes_per_measure[i] *= bpm_stretch;
	}

	std::vector<int> total_metro_qnotes(argc - 1);
	for (size_t i = 0; i < timesigs.size(); ++i)
		total_metro_qnotes[i] = quarter_notes_per_measure[i] * measures[i];

	// a large vector to encompass all time signatures -
	// sum(quarter_notes_per_measure * elapsed_measures)
	int total_quarter_notes = std::accumulate(
	    total_metro_qnotes.begin(), total_metro_qnotes.end(), 0);

	// at every tick, emit a "blank" timbre to the ringbuffers to keep the
	// audio device "spinning" this is _tremendously_ ugly but i had no luck
	// with pausing the streams that should be silenced.
	//
	// if the ringbuffer is not filled at every tick, all my fragile
	// assumptions based on soundio's latency are nullified
	auto empty = metro::timbre::Empty();

	std::vector<metro::Note> metro_vec(total_quarter_notes);

	size_t metro_vec_idx = 0;
	size_t hop;

	for (size_t i = 0; i < timesigs.size(); ++i) {
		std::cout << "Assembling metronome notes for time signature "
		          << timesigs[i] << std::endl;
		hop = common_bpm / bpms[i];
		for (size_t k = 0; k < ( size_t )measures[i]; ++k) {
			( void )k; // k is unused
			for (size_t j = 0; j < ( size_t )quarter_notes_per_measure[i]; ++j) {
				if (timesigs[i].compare("4/4") == 0) {
					if (j == 0)
						metro_vec[metro_vec_idx++] = metro::Note([&]() {
							stream.play_timbres({&strong_downbeat});
						});
					else if (j == hop)
						metro_vec[metro_vec_idx++] = metro::Note(
						    [&]() { stream.play_timbres({&strong_beat}); });
					else if (j == 2 * hop)
						metro_vec[metro_vec_idx++] = metro::Note(
						    [&]() { stream.play_timbres({&weak_downbeat}); });
					else if (j == 3 * hop)
						metro_vec[metro_vec_idx++] = metro::Note(
						    [&]() { stream.play_timbres({&weak_beat}); });
					else
						metro_vec[metro_vec_idx++] = metro::Note(
						    [&]() { stream.play_timbres({&empty}); });
				}
				else if (timesigs[i].compare("3/4") == 0) {
					if (j == 0)
						metro_vec[metro_vec_idx++] = metro::Note([&]() {
							stream.play_timbres({&strong_downbeat});
						});
					else if (j == hop)
						metro_vec[metro_vec_idx++] = metro::Note(
						    [&]() { stream.play_timbres({&weak_beat}); });
					else if (j == 2 * hop)
						metro_vec[metro_vec_idx++] = metro::Note(
						    [&]() { stream.play_timbres({&weak_beat}); });
					else
						metro_vec[metro_vec_idx++] = metro::Note(
						    [&]() { stream.play_timbres({&empty}); });
				}
			}
		}
	}

	auto final_metro = metro::Measure(metro_vec);

	tempo.register_measure(final_metro);
	tempo.start();

	metro::eventloop();

	return 0;
}
