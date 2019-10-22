#ifndef LIBMETRO_H
#define LIBMETRO_H

#include <chrono>
#include <experimental/propagate_const>
#include <list>
#include <memory>
#include <vector>

namespace metro {

// a steady-clock based precise sleep with 1ns precision
// or as close to 1ns as nanosleep gets us on a GPOS
void precise_sleep_us(std::chrono::microseconds dur_us);

const float SampleRateHz = 48000.0;

enum Timbre { Sine, Drum };

class Note {
public:
	Note();
	Note(Timbre timbre, float frequency, float volume);
	float& operator[](size_t index);
	const float& operator[](size_t index) const;
	std::vector<float>& get_frames();

private:
	std::vector<float> frames;
};

class Measure {
public:
	Measure(int num_notes);
	void add_notes(size_t note_index, std::list<Note*> simultaneous_notes);
	Note& operator[](size_t index);
	const Note& operator[](size_t index) const;

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
