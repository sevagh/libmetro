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

		auto poly1 = metro::Measure(2);
		poly1[0] = metro::Note(metro::Note::Timbre::Drum, 54.0, 100.0)
		           + metro::Note(metro::Note::Timbre::Drum, 56.0, 100.0);
		poly1[1] = metro::Note(metro::Note::Timbre::Drum, 56.0, 50.0);

		metronome.add_measure(metro::Measure::NoteLength::Quarter, poly1);

		auto poly2 = metro::Measure(3);
		poly2[0] = metro::Note(metro::Note::Timbre::Drum, 38.0, 100.0)
		           + metro::Note(metro::Note::Timbre::Drum, 42.0, 100.0);
		poly2[1] = metro::Note(metro::Note::Timbre::Drum, 38.0, 50.0);
		poly2[2] = metro::Note(metro::Note::Timbre::Drum, 38.0, 50.0);

		metronome.add_measure(
		    metro::Measure::NoteLength::QuarterTriplet, poly2);
		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
