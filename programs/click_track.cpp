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

	std::cout << "init audio engine" << std::endl;
	std::cout << "Generating tones" << std::endl;

	auto hihat = metro::Note(metro::Timbre::Drum, 42.0, 100.0);
	auto snare = metro::Note(metro::Timbre::Drum, 38.0, 100.0);
	auto bass = metro::Note(metro::Timbre::Drum, 45.0, 100.0);

	metro::Measure snares({snare, snare, snare, snare});
	metro::Measure hihats(4);
	hihats[0] = hihat;
	metro::Measure basses(4);
	basses[2] = bass;

	metronome.add_measure(metro::NoteLength::Quarter, snares);
	metronome.add_measure(metro::NoteLength::Quarter, hihats);
	metronome.add_measure(metro::NoteLength::Quarter, basses);

	metronome.loop();
}
