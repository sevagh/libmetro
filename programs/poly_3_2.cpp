#include "libmetro.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "usage: " << argv[0] << " bpm" << std::endl;
		return 1;
	}

	int bpm = std::stoi(argv[1]);

	try {
		auto metronome = metro::Metronome(bpm);

		auto two_over_four = metro::Measure(2);
		two_over_four[0] = metro::Note(metro::Timbre::Drum, 54.0, 100.0)
		                   + metro::Note(metro::Timbre::Drum, 56.0, 100.0);
		two_over_four[1] = metro::Note(metro::Timbre::Drum, 56.0, 50.0);

		metronome.add_measure(metro::NoteLength::Quarter, two_over_four);

		auto three_over_four = metro::Measure(3);
		three_over_four[0] = metro::Note(metro::Timbre::Drum, 38.0, 100.0)
		                     + metro::Note(metro::Timbre::Drum, 42.0, 100.0);
		three_over_four[1] = metro::Note(metro::Timbre::Drum, 38.0, 50.0);
		three_over_four[2] = metro::Note(metro::Timbre::Drum, 38.0, 50.0);

		metronome.add_measure(
		    metro::NoteLength::QuarterTriplet, three_over_four);
		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
