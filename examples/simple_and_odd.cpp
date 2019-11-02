#include "libmetro.h"
#include <iostream>

//! simple_and_odd is a program for generating simple and odd time signatures
/*!
 * The arguments are 'numerator denonimator bpm', where bpm is the bpm of the
 * quarter note. The numerator must be either == 3 or *not* divisible by 3, and
 * the denominator must be one of 2,4,8,16 which use
 * metro::Measure::NoteLength::{Half, Quarter, Eighth, Sixteenth} respectively.
 *
 * Valid examples are 3/2, 3/4, 4/4, 5/8, 11/4, 13/2.
 */
int main(int argc, char** argv)
{
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " numerator denominator bpm"
		          << std::endl;
		std::cout << "Examples:\n\t2/2, 5/4, 7/4, 3/8, 4/4" << std::endl;
		return 1;
	}

	int num = std::stoi(argv[1]);

	if (num != 3 && num % 3 == 0) {
		std::cerr << "numerator " << num
		          << " is !=3 and divisible by 3 - that's compound"
		          << std::endl;
		return 1;
	}

	int den = std::stoi(argv[2]);
	int bpm = std::stoi(argv[3]);

	metro::Measure::NoteLength l;

	switch (den) {
	case 2:
		l = metro::Measure::Half;
		break;
	case 4:
		l = metro::Measure::Quarter;
		break;
	case 8:
		l = metro::Measure::Eighth;
		break;
	case 16:
		l = metro::Measure::Sixteenth;
		break;
	default:
		std::cerr << num << "/" << den
		          << " is not a simple or odd time signature" << std::endl;
		return 1;
	};

	std::cout << "called with bpm " << bpm << " and time signature " << num
	          << "/" << den << std::endl;

	auto downbeat = metro::Note(metro::Note::Timbre::Drum, 185.0, 100.0)
	                + metro::Note(metro::Note::Timbre::Drum, 207.65, 100.0);
	auto weakbeat = metro::Note(metro::Note::Timbre::Drum, 207.65, 50.0);

	try {
		auto metronome = metro::Metronome(bpm);

		metro::Measure measure(num);
		measure[0] = downbeat;
		for (size_t i = 1; i < ( size_t )num; ++i)
			measure[i] = weakbeat;

		metronome.add_measure(l, measure);
		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
