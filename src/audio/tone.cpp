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
#include <stk/SineWave.h>
#include <thread>
#include <vector>

static bool abs_compare(float a, float b)
{
	return (std::fabs(a) < std::fabs(b));
}

jungle::audio::Tone::Tone(float pitch_hz, float volume_pct)
{
	stk::SineWave sine;
	sine.setFrequency(pitch_hz);
	size_t n_samples = jungle::SampleRateHz;

	// 1 channel, 48,000 samples
	stk::StkFrames frames(n_samples, 1);
	auto frames_ = sine.tick(frames);

	tone = std::vector<float>(
	    ( float* )&frames_[0], ( float* )&frames_[0] + n_samples);

	auto max_elem = *std::max_element(tone.begin(), tone.end(), abs_compare);
	// normalize to 1.0 * volume_pct since libsoundio expects floats between
	// -1.0 and 1.0
	std::transform(tone.begin(), tone.end(), tone.begin(),
	               [volume_pct, max_elem](float elem) {
		               return (volume_pct / 100.0) * (1.0 / max_elem) * elem;
	               });
}

void jungle::audio::Tone::play_on_stream(jungle::audio::Engine::Stream& stream)
{
	assert(jungle::SampleRateHz == stream.outstream->sample_rate);
	assert(tone.size() == jungle::SampleRateHz);

	// fill the stream.ringbuffer with 48,000 samples, which should finish in
	// outstream->software_latency
	// http://libsound.io/doc-1.1.0/structSoundIoOutStream.html#a20aac1422d3cc64b679616bb8447f06d
	char* buf = soundio_ring_buffer_write_ptr(stream.ringbuf);
	size_t fill_count = stream.outstream->software_latency
	                    * stream.outstream->sample_rate
	                    * stream.outstream->bytes_per_frame;
	fill_count = std::min(fill_count, tone.size() * sizeof(float));

	// in case there's stuff in the ringbuffer, we don't want to overflow
	fill_count -= soundio_ring_buffer_fill_count(stream.ringbuf);

	memcpy(buf, tone.data(), fill_count);
	soundio_ring_buffer_advance_write_ptr(stream.ringbuf, fill_count);

	// wait for how long a beep should be
	std::this_thread::sleep_for(
	    std::chrono::duration<float, std::ratio<1, 1>>(stream.latency_s));

	// then, stuff it with 0s to create a smooth transition to
	// silence
	fill_count = soundio_ring_buffer_capacity(stream.ringbuf)
	             - soundio_ring_buffer_fill_count(stream.ringbuf);
	memset(buf, 0, fill_count);
	soundio_ring_buffer_advance_write_ptr(stream.ringbuf, fill_count);
}
