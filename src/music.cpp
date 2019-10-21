#include "libmetro.h"
#include <cassert>
#include <cfloat>
#include <chrono>
#include <cstring>
#include <mutex>
#include <soundio/soundio.h>
#include <stk/Drummer.h>
#include <stk/SineWave.h>
#include <stk/Stk.h>
#include <vector>

static std::once_flag stk_init_flag;

static void stk_init()
{
	std::call_once(stk_init_flag, []() {
		stk::Stk::showWarnings(true);
		stk::Stk::setSampleRate(metro::SampleRateHz);
	});
}

// reversing the freq2midi magic in the stk Drummer code
static float midi2freq(int midi)
{
	return 440.0 * pow(2.0, (float(midi) - 69.0) / 12.0);
}

metro::Note::Note() : frames(std::vector<float>(2 * metro::SampleRateHz)){ }; //empty note

metro::Note::Note(metro::Timbre timbre, float frequency, float volume)
    : frames(std::vector<float>(2 * metro::SampleRateHz))
{
	stk_init();

	switch (timbre) {
		case Sine:
			{
				stk::SineWave sine;
				sine.setFrequency(frequency);

				for (size_t i = 0; i < frames.size(); ++i)
					frames[i] = sine.tick();

				// normalize to 1.0 * volume_pct since libsoundio expects floats between
				// -1.0 and 1.0
				stk::StkFloat max_elem = -DBL_MAX;
				for (size_t i = 0; i < frames.size(); ++i)
					max_elem = std::max(frames[i], max_elem);

				for (size_t i = 0; i < frames.size(); ++i)
					frames[i] = (volume / 100.0) * (1.0 / max_elem) * frames[i];
			}
			break;
		case Drum:
			{
				stk::Drummer drummer;
				drummer.noteOn(midi2freq(frequency), volume / 100.0);
				for (size_t i = 0; i < frames.size(); ++i)
					frames[i] = drummer.tick();

				drummer.noteOff(0.0);
				break;
			}
	};
}

metro::Measure::Measure(int num_notes) :  notes(num_notes) { };

metro::Note& metro::Measure::operator[](size_t index) {
	return notes[index];
}

void metro::Measure::add_notes(size_t note_index, std::list<metro::Note&> simultaneous_notes)
{
	Note total_note;

	for (auto note : notes)
		for (size_t i = 0; i < total_note.frames.size(); ++i)
			total_note += note.frames[i];

	notes[note_index] = total_note;
}
