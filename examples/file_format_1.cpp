#include "libmetro.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " /path/to/metro/txt/file bpm"
		          << std::endl;
		exit(1);
	}

	const char* path = argv[1];
	int bpm = std::stoi(argv[2]);

	try {
		auto metronome = metro::Metronome(bpm);

		// create measure from txt file
		auto measure = metro::Measure(path, metro::Measure::FileFormat::One);
		metronome.add_measure(measure);

		metronome.start_and_loop();
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return 1;
	}
	return 0;
}
