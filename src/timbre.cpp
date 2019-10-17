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

metro::timbre::Sine::Sine(float pitch_hz, float volume_pct)
    : frames(std::vector<float>(2 * metro::SampleRateHz))
{
	stk_init();

	stk::SineWave sine;
	sine.setFrequency(pitch_hz);

	for (size_t i = 0; i < frames.size(); ++i)
		frames[i] = sine.tick();

	// normalize to 1.0 * volume_pct since libsoundio expects floats between
	// -1.0 and 1.0
	stk::StkFloat max_elem = -DBL_MAX;
	for (size_t i = 0; i < frames.size(); ++i)
		max_elem = std::max(frames[i], max_elem);

	for (size_t i = 0; i < frames.size(); ++i)
		frames[i] = (volume_pct / 100.0) * (1.0 / max_elem) * frames[i];
}

// reversing the freq2midi magic in the stk Drummer code
static float midi2freq(int midi)
{
	return 440.0 * pow(2.0, (float(midi) - 69.0) / 12.0);
}

metro::timbre::Drum::Drum(int midi_drum_instrument, float volume_pct)
    : frames(std::vector<float>(2 * metro::SampleRateHz))
{
	stk_init();

	stk::Drummer drummer;
	drummer.noteOn(midi2freq(midi_drum_instrument), volume_pct / 100.0);
	for (size_t i = 0; i < frames.size(); ++i)
		frames[i] = drummer.tick();

	drummer.noteOff(0.0);
}
