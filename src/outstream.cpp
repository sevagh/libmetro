#include "outstream.h"
#include "audioengine.h"
#include "metronome.h"
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <numeric>
#include <soundio/soundio.h>
#include <string>

static float pick_best_latency(std::chrono::microseconds ticker_period)
{
	return (ticker_period.count() / 2.0) / 1000000.0;
}

static void write_callback(struct SoundIoOutStream* outstream,
                           int frame_count_min,
                           int frame_count_max);

metro_private::OutStream::OutStream(metro_private::AudioEngine* parent_engine,
                                    std::chrono::microseconds ticker_period)
    : latency_s(0)
    , parent_engine(parent_engine)
{
	int err;

	auto best_latency = pick_best_latency(ticker_period);

	outstream = soundio_outstream_create(parent_engine->device);

	outstream->format = SoundIoFormatFloat32NE;
	outstream->write_callback = write_callback;

	// try to get the desired latency
	outstream->software_latency = best_latency;
	outstream->sample_rate = metro::SampleRateHz;

	if ((err = soundio_outstream_open(outstream)))
		throw metro::MetroException(std::string("unable to open device: ")
		                            + soundio_strerror(err));

	// use the actual software latency since its not guaranteed
	// that we can set it to whatever we want
	latency_s = outstream->software_latency;

	// adjust the sampling rate to account for the ratio of requested:received
	// software latency
	outstream->sample_rate = metro::SampleRateHz / (latency_s / best_latency);

	int ringbuf_capacity = outstream->software_latency * outstream->sample_rate
	                       * outstream->bytes_per_frame;

	ringbuf
	    = soundio_ring_buffer_create(parent_engine->soundio, ringbuf_capacity);

	if (!ringbuf)
		throw metro::MetroException("unable to create ring buffer: out of "
		                            "memory");

	outstream->userdata = reinterpret_cast<void*>(ringbuf);

	char* buf = soundio_ring_buffer_write_ptr(ringbuf);
	std::memset(buf, 0, ringbuf_capacity);
	soundio_ring_buffer_advance_write_ptr(ringbuf, ringbuf_capacity);
}

metro_private::OutStream::~OutStream()
{
	soundio_ring_buffer_destroy(ringbuf);
	soundio_outstream_destroy(outstream);
}

void metro_private::OutStream::add_measure(metro::Measure& measure)
{
	measures.push_back(measure);
	measure_indices.push_back(0);
}

void metro_private::OutStream::start()
{
	int err;
	compute_notes();
	if ((err = soundio_outstream_start(outstream)))
		throw metro::MetroException(std::string("unable to start device: ")
		                            + soundio_strerror(err));
}

bool metro_private::OutStream::has_measures() { return measures.size() != 0; }

void metro_private::OutStream::compute_notes()
{
	std::vector<size_t> sizes;
	sizes.reserve(measures.size());
	for (auto measure : measures)
		sizes.push_back(measure.size());

	size_t common_size = std::accumulate(
	    sizes.begin(), sizes.end(), 1, std::lcm<size_t, size_t>);

	for (size_t h = 0; h < common_size; ++h) {
		metro::Note total_note;

		for (size_t i = 0; i < measures.size(); ++i) {
			auto note = measures[i][measure_indices[i]];
			for (size_t j = 0; j < total_note.size(); ++j)
				total_note[j] += note[j];

			measure_indices[i]
			    = ++measure_indices[i] % measures[i].size(); // wraparound
		}

		notes.push_back(total_note);
	}
	note_index = 0;
}

void metro_private::OutStream::play_next_note()
{
	auto frames = notes[note_index++ % notes.size()].get_frames();

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
	metro_private::precise_sleep_us(
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
				throw metro::MetroException(std::string("begin write error: ")
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
				throw metro::MetroException(std::string("end write error: ")
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
			throw metro::MetroException(std::string("begin write error: ")
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
			throw metro::MetroException(std::string("end write error: ")
			                            + soundio_strerror(err));

		frames_left -= frame_count;
	}

	soundio_ring_buffer_advance_read_ptr(
	    ring_buffer, read_count * outstream->bytes_per_frame);
}
