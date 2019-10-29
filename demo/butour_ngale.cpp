#include "libmetro.h"
#include <iostream>

//! butour_ngale is my implementation of an African polyrhythm
/*!
 * The song Butour Ngale can be seen here:
 * https://www.youtube.com/watch?v=haGWi5lTibI
 */
int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " bpm" << std::endl;
		return 1;
	}

	int bpm = std::stoi(argv[1]);

	auto downbeat = metro::Note(metro::Note::Timbre::Drum, 185.0, 100.0)
	                + metro::Note(metro::Note::Timbre::Drum, 207.65, 100.0);
	auto mediumbeat = metro::Note(metro::Note::Timbre::Drum, 185.0, 60.0)
	                  + metro::Note(metro::Note::Timbre::Drum, 207.65, 60.0);
	auto weakbeat = metro::Note(metro::Note::Timbre::Drum, 207.65, 50.0);

	try {
		auto metronome = metro::Metronome(bpm);

		// repeat a 6/8 8 times
		metro::Measure measure1(48);
		for (size_t k = 0; k < 48; k += 6) {
			measure1[k] = downbeat;
			for (size_t i = k + 1; i < k + 6; ++i) {
				if (i % 3 == 0)
					measure1[i] = mediumbeat;
				else
					measure1[i] = weakbeat;
			}
		}
		metronome.add_measure(
		    metro::Measure::NoteLength::EighthTriplet, measure1);

		// 16th triplets for the double claps
		metro::Measure measure2(96);

		for (size_t i = 50; i < 72; i += 6) {
			measure2[i] = metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0);
			measure2[i + 2]
			    = metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0);
		}

		// 16th triplets for the single syncopated claps
		for (size_t i = 72; i < 96; i += 4)
			measure2[i] = metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0);

		metronome.add_measure(
		    metro::Measure::NoteLength::SixteenthTriplet, measure2);

		metronome.start_and_loop();
	}
	catch (...) {
		std::cerr << "exception" << std::endl;
		return 1;
	}
	return 0;
}
