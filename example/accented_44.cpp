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

		auto downbeat = metro::Note(metro::Note::Timbre::Sine, 540.0, 100.0);
		auto weakbeat = metro::Note(metro::Note::Timbre::Sine, 350.0, 50.0);
		auto mediumbeat = metro::Note(metro::Note::Timbre::Sine, 440.0, 65.0);

		metro::Measure accented_44(4);
		accented_44[0] = downbeat;
		accented_44[1] = weakbeat;
		accented_44[2] = mediumbeat;
		accented_44[3] = weakbeat;

		metronome.add_measure(metro::Measure::NoteLength::Quarter, accented_44);
		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
