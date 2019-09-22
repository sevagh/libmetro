#include "libjungle.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <future>
#include <iostream>
#include <memory>
#include <soundio/soundio.h>
#include <stdio.h>
#include <vector>

static bool abs_compare(int a, int b) { return (std::abs(a) < std::abs(b)); }

jungle::audio::Tone jungle::audio::generate_tone(float pitch_hz,
                                                 float volume_pct)
{
	size_t size = jungle::SampleRateHz;
	size_t lut_size = size / 4;

	std::vector<int> lut{};
	float* _tone = ( float* )malloc(sizeof(float) * size / 2);

	float delta_phi = pitch_hz * lut_size * 1.0 / jungle::SampleRateHz;
	float phase = 0.0;

	for (int i = 0; i < signed(lut_size); ++i) {
		lut.push_back(( int )roundf(0x7FFF * sinf(2.0 * M_PI * i / lut_size)));
	}

	for (int i = 0; i < signed(size / 2); ++i) {
		_tone[i] = float(lut[( int )phase]);
		phase += delta_phi;
		if (phase >= lut_size)
			phase -= lut_size;
	}

	auto tone = std::vector<float>(_tone, _tone + size / 2);
	auto max_elem = *std::max_element(tone.begin(), tone.end(), abs_compare);

	// normalize to 1.0 * volume_pct
	std::transform(tone.begin(), tone.end(), tone.begin(),
	               [volume_pct, max_elem](float elem) {
		               return (volume_pct / 100.0) * (1.0 / max_elem) * elem;
	               });

	return tone;
}

jungle::audio::Tone jungle::audio::generate_tone(float pitch_hz)
{
	return generate_tone(pitch_hz, 100.0);
}
