#include "libmetro.h"
#include <iostream>

//! compound is a program for generating compound time signatures
/*!
 * The arguments are 'numerator denonimator bpm', where bpm is the bpm of the
 * quarter note. The numerator must be divisible by 3 (and != 3), and the
 * denominator must be one of 4,8,16 which use
 * metro::Measure::NoteLength::{DottedQuarter, DottedEighth,
 * DottedSixteenth} respectively.
 *
 * Valid examples are 6/4, 9/8, 12/16, 15/8.
 */
int main(int argc, char** argv)
{
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " numerator denominator bpm"
		          << std::endl;
		std::cout << "Examples:\n\t6/2, 15/4, 9/8, 12/16" << std::endl;
		return 1;
	}

	int num = std::stoi(argv[1]);

	if (num == 3 || num % 3 != 0) {
		std::cerr << "numerator " << num
		          << " is ==3 or not divisible by 3 - not compound"
		          << std::endl;
		return 1;
	}

	int den = std::stoi(argv[2]);
	int bpm = std::stoi(argv[3]);

	metro::Measure::NoteLength l;
	switch (den) {
	case 4:
		l = metro::Measure::DottedQuarter;
		break;
	case 8:
		l = metro::Measure::DottedEighth;
		break;
	case 16:
		l = metro::Measure::DottedSixteenth;
		break;
	default:
		std::cerr << num << "/" << den << " is not supported" << std::endl;
		return 1;
	};

	std::cout << "called with bpm " << bpm << " and time signature " << num
	          << "/" << den << std::endl;

	auto downbeat = metro::Note(metro::Note::Timbre::Drum, 185.0, 100.0)
	                + metro::Note(metro::Note::Timbre::Drum, 207.65, 100.0);
	auto mediumbeat = metro::Note(metro::Note::Timbre::Drum, 185.0, 60.0)
	                  + metro::Note(metro::Note::Timbre::Drum, 207.65, 60.0);
	auto weakbeat = metro::Note(metro::Note::Timbre::Drum, 207.65, 50.0);

	try {
		auto metronome = metro::Metronome(bpm);

		metro::Measure measure(num);
		measure[0] = downbeat;
		for (size_t i = 1; i < ( size_t )num; ++i) {
			if (i % 3 == 0)
				measure[i] = mediumbeat;
			else
				measure[i] = weakbeat;
		}

		metronome.add_measure(l, measure);
		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
