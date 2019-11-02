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

		auto click = metro::Measure(1);
		click[0] = metro::Note(metro::Note::Timbre::Sine, 440.0, 5.0);

		auto poly1 = metro::Measure(12);
		poly1[0] = metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0)
		           + metro::Note(metro::Note::Timbre::Drum, 92.50, 100.0)
		           + metro::Note(metro::Note::Timbre::Drum, 185.0, 100.0)
		           + metro::Note(metro::Note::Timbre::Drum, 207.65, 100.0);
		poly1[3] = metro::Note(metro::Note::Timbre::Drum, 73.42, 50.0);
		poly1[4] = metro::Note(metro::Note::Timbre::Drum, 207.65, 50.0);
		poly1[6] = metro::Note(metro::Note::Timbre::Drum, 73.42, 50.0);
		poly1[8] = metro::Note(metro::Note::Timbre::Drum, 207.65, 50.0);
		poly1[9] = metro::Note(metro::Note::Timbre::Drum, 73.42, 50.0);

		metronome.add_measure(poly1);
		metronome.add_measure(click);

		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
