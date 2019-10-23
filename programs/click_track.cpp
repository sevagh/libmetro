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

	auto beep = metro::Note(metro::Timbre::Sine, 440.0, 100.0);

	metro::Measure click(1);
	click.add_notes(0, {&beep});

	metronome.add_measure(metro::NoteLength::Quarter, click);
	metronome.loop();
}
