#include "libmetro.h"
#include "stk/Drummer.h"
#include "stk/SineWave.h"
#include "stk/Stk.h"
#include <cfloat>
#include <cmath>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

static std::once_flag stk_init_flag;

static void stk_init()
{
	std::call_once(stk_init_flag, []() {
		stk::Stk::showWarnings(true);
		stk::Stk::setSampleRate(metro::SampleRateHz);
	});
}

static void normalize(std::vector<float>& frames, float ratio)
{
	// normalize to 1.0 * volume_pct since libsoundio expects floats
	// between -1.0 and 1.0
	float max_elem = -FLT_MAX;
	for (size_t i = 0; i < frames.size(); ++i)
		max_elem = std::max(frames[i], max_elem);

	float min_elem = FLT_MAX;
	for (size_t i = 0; i < frames.size(); ++i)
		min_elem = std::min(frames[i], min_elem);

	max_elem = std::max(std::abs(min_elem), max_elem);

	for (size_t i = 0; i < frames.size(); ++i)
		frames[i] = (ratio / max_elem) * frames[i];
}

static void populate_frames(std::vector<float>& frames,
                            metro::Note::Timbre timbre,
                            float frequency,
                            float volume)
{
	stk_init();

	switch (timbre) {
	case metro::Note::Timbre::Sine: {
		stk::SineWave sine;
		sine.setFrequency(frequency);

		for (size_t i = 0; i < frames.size(); ++i)
			frames[i] = sine.tick();

		normalize(frames, volume / 100.0);
	} break;
	case metro::Note::Timbre::Drum: {
		stk::Drummer drummer;
		drummer.noteOn(frequency, volume / 100.0);
		for (size_t i = 0; i < frames.size(); ++i)
			frames[i] = drummer.tick();

		drummer.noteOff(0.0);
	} break;
	};
}

metro::Note::Note(metro::Note::Timbre timbre, float frequency, float volume)
    : frames(std::vector<float>(2 * metro::SampleRateHz))
{
	populate_frames(frames, timbre, frequency, volume);
}

metro::Note::Note(std::string triplet)
    : frames(std::vector<float>(2 * metro::SampleRateHz))
{
	std::string substr;
	std::stringstream nested_ss(triplet);

	metro::Note::Timbre timbre;
	float freq;
	float vol;

	int j = 0;
	while (std::getline(nested_ss, substr, ',')) {
		if (j == 0) {
			if (substr.compare("sine") == 0) {
				timbre = metro::Note::Timbre::Sine;
			}
			else if (substr.compare("drum") == 0) {
				timbre = metro::Note::Timbre::Drum;
			}
			else {
				throw std::runtime_error("invalid timbre requested");
			}
		}
		else if (j == 1) {
			freq = std::stof(substr);
		}
		else if (j == 2) {
			vol = std::stof(substr);
		}
		j++;
	}

	populate_frames(frames, timbre, freq, vol);
}
