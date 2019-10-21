#ifndef LIBMETRO_H
#define LIBMETRO_H

#include <chrono>
#include <vector>
#include <experimental/propagate_const>
#include <memory>
#include <list>

namespace metro {

// a steady-clock based precise sleep with 1ns precision
// or as close to 1ns as nanosleep gets us on a GPOS
void precise_sleep_us(std::chrono::microseconds dur_us);

const float SampleRateHz = 48000.0;

enum Timbre { Sine, Drum };

class Note {
public:
	Note();
	std::vector<float> frames;
	Note(Timbre timbre, float frequency, float volume);
};

class Measure {
public:
	Measure(int num_notes);
	void add_notes(size_t note_index, std::list<Note&> simultaneous_notes);
	Note& operator[](size_t index);

private:
	std::vector<Note> notes;
};

enum NoteLength { Half, Quarter, Eighth, Sixteenth };

class Metronome {
public:
	Metronome(int bpm);

	void add_measure(NoteLength note_length, Measure& measure);
	void loop();

// https://en.cppreference.com/w/cpp/language/pimpl
private:
	class impl;
	std::experimental::propagate_const<std::unique_ptr<impl>> p_impl;
};
}; // namespace metro

#endif /* LIBMETRO_H */
