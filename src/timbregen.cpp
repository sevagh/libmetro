#include "timbregen.h"
#include "libmetro.h"
#include <cfloat>
#include <cmath>
#include <vector>

// generate sine waves with the following timbres
// enum Timbre { Sine, Square, Sawtooth, Triangle };

metro::Note::Note(metro::Timbre timbre, float frequency, float volume)
    : frames(std::vector<float>(2 * metro::SampleRateHz))
{
	metro_private::TimbreGen gen(timbre, volume / 100.0, frequency, 0.0);
	for (size_t t = 0; t < frames.size(); ++t)
		frames[t] = gen.next_sample(t);
}

metro_private::TimbreGen::TimbreGen(metro::Timbre timbre,
                                    float level,
                                    float frequency,
                                    float phase)
    : timbre(timbre)
    , level(level)
    , frequency(frequency)
    , phase(phase)
{
}

// attribution:
// https://medium.com/@audiowaves/basic-sound-waves-with-c-and-juce-50ec9f0bfe5c
float metro_private::TimbreGen::next_sample(size_t time_point)
{
	float sample = 0.0;

	switch (timbre) {
	case metro::Timbre::Sine:
		sample = sine_sample(time_point);
		break;
	case metro::Timbre::Square:
		sample = square_sample(time_point);
		break;
	case metro::Timbre::Sawtooth:
		sample = sawtooth_sample(time_point);
		break;
	case metro::Timbre::Triangle:
		sample = triangle_sample(time_point);
		break;
	default:
		break;
	}
	return sample;
}

float metro_private::TimbreGen::sine_sample(size_t t)
{
	return level * sin(2.0 * M_PI * frequency * ( float )t + phase);
}

float metro_private::TimbreGen::square_sample(size_t t)
{
	float value = sin(2.0 * M_PI * frequency * ( float )t + phase);
	value = (value >= 0.0) ? 1.0 : -1.0;
	return level * value;
}

float metro_private::TimbreGen::triangle_sample(size_t t)
{
	float res = 0.0;
	float full_period_time = 1.0 / frequency;
	float local_time = fmod(( float )t, full_period_time);

	float value = local_time / full_period_time;

	if (value < 0.25)
		res = value * 4.0;
	else if (value < 0.75)
		res = 2.0 - (value * 4.0);
	else
		res = value * 4.0 - 4.0;

	return level * res;
}

float metro_private::TimbreGen::sawtooth_sample(size_t t)
{
	float full_period_time = 1.0 / frequency;
	float local_time = fmod(( float )t, full_period_time);

	return level * ((local_time / full_period_time) * 2.0 - 1.0);
}
