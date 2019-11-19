#include "libmetro.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0]
		          << " /path/to/metro/txt/file file_format bpm" << std::endl;
		exit(1);
	}

	const char* path = argv[1];
	int file_format = std::stoi(argv[2]);
	int bpm = std::stoi(argv[3]);

	auto ff = metro::Measure::FileFormat::One;

	switch (file_format) {
	case 0:
		break;
	case 1:
		ff = metro::Measure::FileFormat::Two;
		break;
	default:
		std::cerr << "File formats are 0 (::One) and 1 (::Two)" << std::endl;
		exit(1);
	};

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
