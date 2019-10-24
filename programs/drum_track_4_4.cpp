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

	metro::Measure beat22(4);
	beat22.add_notes(0, {&hihat, &snare});
	beat22.add_notes(1, {&hihat});
	beat22.add_notes(2, {&hihat, &bass});
	beat22.add_notes(3, {&hihat});

	metronome.add_measure(metro::NoteLength::Quarter, beat22);
	metronome.loop();
}
