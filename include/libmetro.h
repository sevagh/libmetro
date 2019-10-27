#ifndef LIBMETRO_H
#define LIBMETRO_H

#include <chrono>
#include <vector>

// forward declare the private implementation of Metronome
namespace metro_private {
class MetronomePrivate;
};

namespace metro {

// a steady-clock based precise sleep with 1ns precision
// or as close to 1ns as nanosleep gets us on a GPOS
void precise_sleep_us(std::chrono::microseconds dur_us);

const float SampleRateHz = 48000.0;

enum Timbre { Sine, Drum };

class Note {
public:
	Note()
	    : frames(std::vector<float>(2 * SampleRateHz)){};

	// defined in src/timbregen.cpp
	Note(Timbre timbre, float frequency, float volume);

	float& operator[](size_t index) { return frames[index]; };
	const float& operator[](size_t index) const { return frames[index]; };
	std::vector<float>& get_frames() { return frames; };
	size_t size() { return frames.size(); };

	void normalize(float ratio);

	Note operator+(const Note& other)
	{
		metro::Note ret;
		for (size_t i = 0; i < ret.size(); ++i)
			ret[i] = (*this)[i] + other[i];
		ret.normalize(1.0);
		return ret;
	}

private:
	std::vector<float> frames;
};

class Measure {
public:
	Measure(int num_notes)
	    : notes(num_notes){};
	Note& operator[](size_t index) { return notes[index]; };
	const Note& operator[](size_t index) const { return notes[index]; };
	std::vector<Note>& get_notes() { return notes; };
	size_t size() { return notes.size(); };

private:
	std::vector<Note> notes;
};

enum NoteLength {
	Half,
	Quarter,
	QuarterTriplet,
	Eighth,
	EighthTriplet,
	Sixteenth
};

class Metronome {
public:
	Metronome(int bpm);
	~Metronome();

	void add_measure(NoteLength note_length, Measure& measure);
	void start();          // for manual loop control
	void start_and_loop(); // start + loop

private:
	metro_private::MetronomePrivate*
	    p_impl; // https://en.cppreference.com/w/cpp/language/pimpl
};
}; // namespace metro

#endif /* LIBMETRO_H */
