#include "libmetro.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " bpm" << std::endl;
		exit(1);
	}

	int bpm = std::stoi(argv[1]);

	try {
		auto metronome = metro::Metronome(bpm);
		// auto beep = metro::Note(metro::Timbre::Sine, 440.0, 100.0);
		// auto beep = metro::Note(metro::Timbre::Square, 440.0, 100.0);
		// auto beep = metro::Note(metro::Timbre::Sawtooth, 440.0, 100.0);
		auto beep = metro::Note(metro::Timbre::Triangle, 440.0, 100.0);

		metro::Measure click(1);
		click[0] = beep;

		metronome.add_measure(metro::NoteLength::Quarter, click);
		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
