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

enum Timbre { Sine, Square, Sawtooth, Triangle };

class Note {
public:
	Note();
	Note(Timbre timbre, float frequency, float volume);
	float& operator[](size_t index);
	Note operator+(const Note& other);
	const float& operator[](size_t index) const;
	std::vector<float>& get_frames();
	size_t size();

private:
	std::vector<float> frames;
};

class Measure {
public:
	Measure(int num_notes);
	Note& operator[](size_t index);
	const Note& operator[](size_t index) const;
	std::vector<Note>& get_notes();
	size_t size();
	void toggle_mute(bool mute);
	bool is_muted();

private:
	bool muted;
	std::vector<Note> notes;
};

enum NoteLength { Half, Quarter, Eighth, Sixteenth };

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
