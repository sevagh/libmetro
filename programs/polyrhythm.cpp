#include "libmetro.h"
#include <iostream>

//! polyrhythm is a program for generating polyrhthms
/*!
 * The arguments are 'po:ly bpm', where bpm is the bpm of the quarter note.
 *
 * 'po:ly' is of the form 3:2, 5:3, etc. Unlike simple_and_odd.cpp or
 * compound.cpp, these are not computed automatically, but hardcoded. The
 * supported polyrhythms are:
 *
 * - 3:2
 * - 4:3
 * - 5:3
 * - 7:3
 * - 7:5
 *
 * **N.B.** I cannot guarantee these are musically correct. My copout is that
 * these are just examples, and if you wanted to do it better, libmetro would
 * empower you to do so.
 */
int main(int argc, char** argv)
{
	if (argc != 3) {
		std::cerr << "usage: " << argv[0] << " po:ly bpm" << std::endl;
		std::cerr << "(max of two overlaid rhythms, no more)" << std::endl;
		std::cerr << "some of these may be opinion vs. fact" << std::endl;
		return 1;
	}

	std::string polyrhythm(argv[1]);
	int bpm = std::stoi(argv[2]);
	int poly1_count, poly2_count = 0;
	metro::Measure::NoteLength poly1_len, poly2_len;

	auto metronome = metro::Metronome(bpm);

	if (polyrhythm.compare("3:2") == 0) {
		poly1_count = 2;
		poly2_count = 3;
		poly1_len = metro::Measure::NoteLength::Quarter;
		poly2_len = metro::Measure::NoteLength::QuarterTriplet;
		std::cout << "overlaying 2/4 and 3/(4 triplet)" << std::endl;
	}
	else if (polyrhythm.compare("4:3") == 0) {
		poly1_count = 3;
		poly2_count = 4;
		poly1_len = metro::Measure::NoteLength::Quarter;
		poly2_len = metro::Measure::NoteLength::QuarterTriplet;
		std::cout << "overlaying 3/4 and 4/(4 triplet)" << std::endl;
	}
	else if (polyrhythm.compare("5:3") == 0) {
		poly1_count = 3;
		poly2_count = 5;
		poly1_len = metro::Measure::NoteLength::Quarter;
		poly2_len = metro::Measure::NoteLength::QuarterTriplet;
		std::cout << "overlaying 3/4 and 5/(4 triplet)" << std::endl;
	}
	else if (polyrhythm.compare("7:3") == 0) {
		poly1_count = 3;
		poly2_count = 7;
		poly1_len = metro::Measure::NoteLength::Quarter;
		poly2_len = metro::Measure::NoteLength::EighthTriplet;
		std::cout << "overlaying 3/4 and 7/(8 triplet)" << std::endl;
	}
	else if (polyrhythm.compare("7:5") == 0) {
		poly1_count = 5;
		poly2_count = 7;
		poly1_len = metro::Measure::NoteLength::QuarterTriplet;
		poly2_len = metro::Measure::NoteLength::EighthTriplet;
		std::cout << "overlaying 5/(4 triplet) and 7/(8 triplet)" << std::endl;
	}
	else {
		std::cerr << polyrhythm << " is unsupported" << std::endl;
		return 1;
	}

	auto downbeat1 = metro::Note(metro::Note::Timbre::Drum, 185.0, 100.0)
	                 + metro::Note(metro::Note::Timbre::Drum, 207.65, 100.0);
	auto weakbeat1 = metro::Note(metro::Note::Timbre::Drum, 207.65, 50.0);

	auto downbeat2 = metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0)
	                 + metro::Note(metro::Note::Timbre::Drum, 92.50, 100.0);
	auto weakbeat2 = metro::Note(metro::Note::Timbre::Drum, 73.42, 50.0);

	try {
		auto poly1 = metro::Measure(poly1_count);
		poly1[0] = downbeat1;
		for (size_t i = 1; i < ( size_t )poly1_count; ++i)
			poly1[i] = weakbeat1;

		metronome.add_measure(poly1_len, poly1);

		auto poly2 = metro::Measure(poly2_count);
		poly2[0] = downbeat2;
		for (size_t i = 1; i < ( size_t )poly2_count; ++i)
			poly2[i] = weakbeat2;

		metronome.add_measure(poly2_len, poly2);

		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
