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
#include <stk/Drummer.h>
#include <stk/SineWave.h>
#include <thread>
#include <vector>

jungle::audio::timbre::Pulse::Pulse(float pitch_hz, float volume_pct)
{
	stk::SineWave sine;
	sine.setFrequency(pitch_hz);

	// 2 channels
	stk::StkFrames frames_(jungle::SampleRateHz, 2);
	sine.tick(frames_, 0);
	sine.tick(frames_, 1);

	frames = frames_;

	// normalize to 1.0 * volume_pct since libsoundio expects floats between
	// -1.0 and 1.0
	stk::StkFloat max_elem = -DBL_MAX;
	for (size_t i = 0; i < frames.size(); ++i)
		max_elem = std::max(frames[i], max_elem);

	for (size_t i = 0; i < frames.size(); ++i)
		frames[i] = (volume_pct / 100.0) * (1.0 / max_elem) * frames[i];
}

jungle::audio::timbre::DrumTap::DrumTap(float volume_pct)
{
	stk::Drummer drummer;

	// 2 channels
	stk::StkFrames frames_(jungle::SampleRateHz, 2);

	// 138.0 sounds vaguely drum-like
	drummer.noteOn(138.0, 1.0);
	drummer.tick(frames_, 0);
	drummer.tick(frames_, 1);
	drummer.noteOff(0.0);

	frames = frames_;

	// normalize to 1.0 * volume_pct since libsoundio expects floats between
	// -1.0 and 1.0
	stk::StkFloat max_elem = -DBL_MAX;
	for (size_t i = 0; i < frames.size(); ++i)
		max_elem = std::max(frames[i], max_elem);

	for (size_t i = 0; i < frames.size(); ++i)
		frames[i] = (volume_pct / 100.0) * (1.0 / max_elem) * frames[i];
}

void jungle::audio::timbre::play_on_stream(jungle::audio::Engine::Stream& stream,
                                           jungle::audio::timbre::Timbre& timbre)
{
	auto frames = timbre.get_frames();

	assert(jungle::SampleRateHz == stream.outstream->sample_rate);
	assert(sizeof(stk::StkFloat) == 4);
	assert(frames.size() == 2 * jungle::SampleRateHz);

	// fill the stream.ringbuffer with 2*48,000 samples, which should finish in
	// outstream->software_latency
	// http://libsound.io/doc-1.1.0/structSoundIoOutStream.html#a20aac1422d3cc64b679616bb8447f06d
	char* buf = soundio_ring_buffer_write_ptr(stream.ringbuf);
	size_t fill_count = stream.outstream->software_latency
	                    * stream.outstream->sample_rate
	                    * stream.outstream->bytes_per_frame;
	fill_count = std::min(fill_count, frames.size() * sizeof(stk::StkFloat));

	// in case there's stuff in the ringbuffer, we don't want to overflow
	fill_count -= soundio_ring_buffer_fill_count(stream.ringbuf);

	memcpy(buf, &frames[0], fill_count);
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
