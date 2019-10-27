#include "libmetro.h"
#include "stk/Drummer.h"
#include "stk/SineWave.h"
#include "stk/Stk.h"
#include <cfloat>
#include <cmath>
#include <mutex>
#include <vector>

// generate sine waves with the following timbres
// enum Timbre { Sine, Drum };
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

void metro::Note::normalize(float ratio)
{
	// normalize to 1.0 * volume_pct since libsoundio expects floats
	// between -1.0 and 1.0
	float max_elem = -FLT_MAX;
	for (size_t i = 0; i < frames.size(); ++i)
		max_elem = std::max(frames[i], max_elem);

	stk::StkFloat min_elem = FLT_MAX;
	for (size_t i = 0; i < frames.size(); ++i)
		min_elem = std::min(frames[i], min_elem);

	max_elem = std::max(std::abs(min_elem), max_elem);

	for (size_t i = 0; i < frames.size(); ++i)
		frames[i] = (ratio / max_elem) * frames[i];
}

metro::Note::Note(metro::Timbre timbre, float frequency, float volume)
    : frames(std::vector<float>(2 * metro::SampleRateHz))
{
	stk_init();

	switch (timbre) {
	case Sine: {
		stk::SineWave sine;
		sine.setFrequency(frequency);

		for (size_t i = 0; i < frames.size(); ++i)
			frames[i] = sine.tick();

		normalize(volume / 100.0);
	} break;
	case Drum: {
		stk::Drummer drummer;
		drummer.noteOn(midi2freq(frequency), volume / 100.0);
		for (size_t i = 0; i < frames.size(); ++i)
			frames[i] = drummer.tick();

		drummer.noteOff(0.0);
	} break;
	};
}
