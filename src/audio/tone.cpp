#include "libjungle.h"
#include <algorithm>
#include <cassert>
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
	size_t size = 2 * jungle::SampleRateHz;
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

void jungle::audio::Tone::play_on_stream(jungle::audio::Engine::Stream& stream)
{
	assert(jungle::SampleRateHz == stream.outstream->sample_rate);
	assert(tone.size() == jungle::SampleRateHz);

	auto ringbuf = stream.ringbuf;
	char* buf = soundio_ring_buffer_write_ptr(ringbuf);

	// fill the ringbuffer with 48,000 samples, which should finish in
	// outstream->software_latency
	// http://libsound.io/doc-1.1.0/structSoundIoOutStream.html#a20aac1422d3cc64b679616bb8447f06d
	size_t fill_count = stream.outstream->software_latency
	                    * stream.outstream->sample_rate
	                    * stream.outstream->bytes_per_frame;
	fill_count = std::min(fill_count, tone.size() * sizeof(float));
	memcpy(buf, tone.data(), fill_count);
	soundio_ring_buffer_advance_write_ptr(ringbuf, fill_count);

	// wait for how long a beep should be
	std::this_thread::sleep_for(std::chrono::duration<float, std::ratio<1, 1>>(
	    stream.outstream->software_latency));

	// then, fill it with many many many 0s to create a smooth transition to
	// silence
	fill_count = soundio_ring_buffer_capacity(ringbuf);
	memset(buf, 0, fill_count);
	soundio_ring_buffer_advance_write_ptr(ringbuf, fill_count);
}
