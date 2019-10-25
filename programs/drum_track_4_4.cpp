#include "libmetro.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " bpm" << std::endl;
		exit(1);
	}

	int bpm = std::stoi(argv[1]);
	auto metronome = metro::Metronome(bpm);

	try {
		auto hihat = metro::Note(metro::Timbre::Drum, 42.0, 100.0);
		auto snare = metro::Note(metro::Timbre::Drum, 38.0, 100.0);
		auto bass = metro::Note(metro::Timbre::Drum, 45.0, 100.0);

		metro::Measure beat44(4);
		beat44[0] = hihat + snare;
		beat44[1] = hihat;
		beat44[2] = hihat + bass;
		beat44[3] = hihat;

		metronome.add_measure(metro::NoteLength::Quarter, beat44);
		metronome.loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		std::exit(1);
	}
	return 0;
}
