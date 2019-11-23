#include "libmetro.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 3 || argc > 4) {
		std::cerr << "Usage: " << argv[0]
		          << " /path/to/metro/txt/file bpm [file_format]" << std::endl;
		exit(1);
	}

	const char* path = argv[1];
	int bpm = std::stoi(argv[2]);

	auto ff = metro::Measure::FileFormat::One;
	if (argc == 4) {
		switch (std::stoi(argv[3])) {
		case 1:
			break;
		case 2:
			ff = metro::Measure::FileFormat::Two;
			break;
		default:
			std::cerr << "File formats are 1 and 2" << std::endl;
			exit(1);
		};
	}

	try {
		auto metronome = metro::Metronome(bpm);

		// create measure from txt file
		auto measure = metro::Measure(path, ff);
		metronome.add_measure(measure);

		metronome.start_and_loop();
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return 1;
	}
	return 0;
}
