#include "libjungle.h"
#include <algorithm>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstring>
#include <future>
#include <iostream>
#include <memory>
#include <soundio/soundio.h>
#include <stdio.h>
#include <thread>
#include <vector>

static bool abs_compare(int a, int b) { return (std::abs(a) < std::abs(b)); }

jungle::audio::Tone::Tone(float pitch_hz, float volume_pct)
    : pitch_hz(pitch_hz)
    , volume_pct(volume_pct)
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

	tone = std::vector<float>(_tone, _tone + size / 2);
	auto max_elem = *std::max_element(tone.begin(), tone.end(), abs_compare);

	// normalize to 1.0 * volume_pct
	std::transform(tone.begin(), tone.end(), tone.begin(),
	               [volume_pct, max_elem](float elem) {
		               return (volume_pct / 100.0) * (1.0 / max_elem) * elem;
	               });
}

void jungle::audio::Tone::play_on_stream(jungle::audio::Engine::Stream& stream,
                                         int duration_us)
{
	auto ringbuf = stream.ringbuf;
	char* buf = soundio_ring_buffer_write_ptr(ringbuf);
	// int fill_count = stream.outstream->software_latency
	//                 * (duration_us / 1000000.0 *
	//                 stream.outstream->sample_rate)
	//                 * stream.outstream->bytes_per_frame;
	int fill_count = (duration_us / 1000000.0 * stream.outstream->sample_rate)
	                 * sizeof(float);
	std::cout << "duration us: " << duration_us << std::endl;
	std::cout << "duration us/1000000.0: " << duration_us / 1000000.0
	          << std::endl;
	std::cout << "tone size: " << tone.size() << std::endl;
	std::cout << "desired fill count: " << fill_count << std::endl;
	memcpy(buf, tone.data(), fill_count);
	soundio_ring_buffer_advance_write_ptr(ringbuf, fill_count);
}
