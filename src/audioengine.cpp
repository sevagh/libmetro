#include "libmetro.h"
#include "libmetro_private.h"
#include <cassert>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <soundio/soundio.h>
#include <string>

metro_private::AudioEngine::AudioEngine()
{
	int err;

	soundio = soundio_create();
	if (!soundio)
		throw std::runtime_error("out of memory");

	if ((err = soundio_connect(soundio)))
		throw std::runtime_error(std::string("error connecting: ")
		                         + soundio_strerror(err));

	soundio_flush_events(soundio);

	int default_out_device_index = soundio_default_output_device_index(soundio);
	if (default_out_device_index < 0)
		throw std::runtime_error("no output device found");

	device = soundio_get_output_device(soundio, default_out_device_index);
	if (!device)
		throw std::runtime_error("out of memory");
}

void metro_private::AudioEngine::eventloop()
{
	for (;;)
		soundio_wait_events(soundio);
}

metro_private::AudioEngine::~AudioEngine()
{
	soundio_device_unref(device);
	soundio_destroy(soundio);
}

static float pick_best_latency(std::chrono::microseconds ticker_period)
{
	return (ticker_period.count() / 2.0) / 1000000.0;
}

metro_private::AudioEngine::OutStream metro_private::AudioEngine::new_outstream(
    std::chrono::microseconds ticker_period)
{
	float best_latency_s = pick_best_latency(ticker_period);
	return metro_private::AudioEngine::OutStream(this, best_latency_s);
}

static void write_callback(struct SoundIoOutStream* outstream,
                           int frame_count_min,
                           int frame_count_max);

metro_private::AudioEngine::OutStream::OutStream(
    metro_private::AudioEngine* parent_engine,
    float latency_s)
    : latency_s(latency_s)
{
	int err;

	outstream = soundio_outstream_create(parent_engine->device);

	outstream->format = SoundIoFormatFloat32NE;
	outstream->write_callback = write_callback;

	outstream->software_latency = latency_s;
	outstream->sample_rate = metro::SampleRateHz;

	if ((err = soundio_outstream_open(outstream)))
		throw std::runtime_error(std::string("unable to open device: ")
		                         + soundio_strerror(err));

	int ringbuf_capacity = outstream->software_latency * outstream->sample_rate
	                       * outstream->bytes_per_frame;
	ringbuf
	    = soundio_ring_buffer_create(parent_engine->soundio, ringbuf_capacity);

	if (!ringbuf)
		throw std::runtime_error("unable to create ring buffer: out of "
		                         "memory");

	outstream->userdata = reinterpret_cast<void*>(ringbuf);

	char* buf = soundio_ring_buffer_write_ptr(ringbuf);
	std::memset(buf, 0, ringbuf_capacity);
	soundio_ring_buffer_advance_write_ptr(ringbuf, ringbuf_capacity);

	if ((err = soundio_outstream_start(outstream)))
		throw std::runtime_error(std::string("unable to start device: ")
		                         + soundio_strerror(err));
}

metro_private::AudioEngine::OutStream::~OutStream()
{
	soundio_ring_buffer_destroy(ringbuf);
	soundio_outstream_destroy(outstream);
}

void metro_private::AudioEngine::OutStream::add_measure(metro::Measure& measure)
{
	measures.push_back(measure);
	measure_indices.push_back(0);
}

void metro_private::AudioEngine::OutStream::play_next_note()
{
	std::vector<float> frames(2 * metro::SampleRateHz);

	for (size_t i = 0; i < measures.size(); ++i) {
		auto measure_idx = measure_indices[i];
		measure_indices[i]
		    = ++measure_indices[i] % measure_indices.size(); // wraparound
		auto note = measures[i][measure_idx];
		for (size_t j = 0; j < frames.size(); ++j)
			frames[j] += note[j];
	}

	// fill the stream.ringbuffer with 2*48,000 samples, which should finish in
	// outstream->software_latency
	// http://libsound.io/doc-1.1.0/structSoundIoOutStream.html#a20aac1422d3cc64b679616bb8447f06d
	char* buf = soundio_ring_buffer_write_ptr(ringbuf);
	size_t fill_count = outstream->software_latency * outstream->sample_rate
	                    * outstream->bytes_per_frame;

	fill_count = std::min(fill_count, frames.size() * sizeof(float));

	// in case there's stuff in the ringbuffer, we don't want to overflow
	fill_count -= soundio_ring_buffer_fill_count(ringbuf);

	std::memcpy(buf, frames.data(), fill_count);

	soundio_ring_buffer_advance_write_ptr(ringbuf, fill_count);

	// wait for how long a tick should be
	metro::precise_sleep_us(
	    std::chrono::duration_cast<std::chrono::microseconds>(
	        std::chrono::duration<float, std::ratio<1, 1>>(latency_s)));

	// then, stuff it with 0s to create a smooth transition to
	// silence
	fill_count = soundio_ring_buffer_capacity(ringbuf)
	             - soundio_ring_buffer_fill_count(ringbuf);
	std::memset(buf, 0, fill_count);
	soundio_ring_buffer_advance_write_ptr(ringbuf, fill_count);
}

static void write_callback(struct SoundIoOutStream* outstream,
                           int frame_count_min,
                           int frame_count_max)
{
	struct SoundIoChannelArea* areas;
	int frames_left;
	int frame_count;
	int err;

	struct SoundIoRingBuffer* ring_buffer
	    = reinterpret_cast<struct SoundIoRingBuffer*>(outstream->userdata);

	char* read_ptr = soundio_ring_buffer_read_ptr(ring_buffer);
	int fill_bytes = soundio_ring_buffer_fill_count(ring_buffer);
	int fill_count = fill_bytes / outstream->bytes_per_frame;

	if (frame_count_min > fill_count) {
		// Ring buffer does not have enough data, fill with zeroes.
		frames_left = frame_count_min;
		for (;;) {
			frame_count = frames_left;
			if (frame_count <= 0)
				return;
			if ((err = soundio_outstream_begin_write(
			         outstream, &areas, &frame_count)))
				throw std::runtime_error(std::string("begin write error: ")
				                         + soundio_strerror(err));
			if (frame_count <= 0)
				return;
			for (int frame = 0; frame < frame_count; frame += 1) {
				for (int ch = 0; ch < outstream->layout.channel_count; ch += 1) {
					std::memset(areas[ch].ptr, 0, outstream->bytes_per_sample);
					areas[ch].ptr += areas[ch].step;
				}
			}
			if ((err = soundio_outstream_end_write(outstream)))
				throw std::runtime_error(std::string("end write error: ")
				                         + soundio_strerror(err));
			frames_left -= frame_count;
		}
	}

	int read_count = std::min(frame_count_max, fill_count);
	frames_left = read_count;

	while (frames_left > 0) {
		int frame_count = frames_left;

		if ((err
		     = soundio_outstream_begin_write(outstream, &areas, &frame_count)))
			throw std::runtime_error(std::string("begin write error: ")
			                         + soundio_strerror(err));

		if (frame_count <= 0)
			break;

		for (int frame = 0; frame < frame_count; frame += 1) {
			for (int ch = 0; ch < outstream->layout.channel_count; ch += 1) {
				std::memcpy(
				    areas[ch].ptr, read_ptr, outstream->bytes_per_sample);
				areas[ch].ptr += areas[ch].step;
				read_ptr += outstream->bytes_per_sample;
			}
		}

		if ((err = soundio_outstream_end_write(outstream)))
			throw std::runtime_error(std::string("end write error: ")
			                         + soundio_strerror(err));

		frames_left -= frame_count;
	}

	soundio_ring_buffer_advance_read_ptr(
	    ring_buffer, read_count * outstream->bytes_per_frame);
}
