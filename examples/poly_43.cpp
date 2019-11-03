#include "libmetro.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 6) {
		std::cerr << "usage: " << argv[0]
		          << " measures1 measures2 measures1+2 nmeasures0 bpm"
		          << std::endl;
		return 1;
	}

	int n_measures1 = std::stoi(argv[1]);
	int n_measures2 = std::stoi(argv[2]);
	int n_measures12 = std::stoi(argv[3]);
	int n_measures0 = std::stoi(argv[4]);
	int bpm = std::stoi(argv[5]);

	// timbre1, snare + hihat
	auto downbeat1 = metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0)
	                 + metro::Note(metro::Note::Timbre::Drum, 92.50, 100.0);
	auto weakbeat1 = metro::Note(metro::Note::Timbre::Drum, 73.42, 50.0);

	// timbre2, cowbell + tambourine
	auto downbeat2 = metro::Note(metro::Note::Timbre::Drum, 185.0, 100.0)
	                 + metro::Note(metro::Note::Timbre::Drum, 207.65, 100.0);
	auto weakbeat2 = metro::Note(metro::Note::Timbre::Drum, 207.65, 50.0);

	try {
		auto metronome = metro::Metronome(bpm);

		auto click = metro::Measure(1);
		click[0] = metro::Note(metro::Note::Timbre::Sine, 440.0, 5.0);

		auto poly1 = metro::Measure(12 * n_measures1 + 12 * n_measures2
		                            + 12 * n_measures12 + 12 * n_measures0);

		// measures with just the first rhythm
		for (size_t i = 0; i < ( size_t )n_measures1; ++i) {
			size_t base_idx = 12 * i;
			poly1[base_idx] = downbeat1;
			poly1[base_idx + 3] = weakbeat1;
			poly1[base_idx + 6] = weakbeat1;
			poly1[base_idx + 9] = weakbeat1;
		}

		// measures with just the second rhythm
		for (size_t i = 0; i < ( size_t )n_measures2; ++i) {
			size_t base_idx = 12 * n_measures1 + 12 * i;
			poly1[base_idx] = downbeat2;
			poly1[base_idx + 4] = weakbeat2;
			poly1[base_idx + 8] = weakbeat2;
		}

		// measures with both
		for (size_t i = 0; i < ( size_t )n_measures12; ++i) {
			size_t base_idx = 12 * n_measures1 + 12 * n_measures2 + 12 * i;
			poly1[base_idx] = downbeat1 + downbeat2;
			poly1[base_idx + 3] = weakbeat1;
			poly1[base_idx + 4] = weakbeat2;
			poly1[base_idx + 6] = weakbeat1;
			poly1[base_idx + 8] = weakbeat2;
			poly1[base_idx + 9] = weakbeat1;
		}

		metronome.add_measure(poly1);
		metronome.add_measure(click);

		metronome.start_and_loop();
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return 1;
	}
	return 0;
}
