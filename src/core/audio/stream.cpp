#include "libjungle/libjungle.h"
#include <cstring>
#include <soundio/soundio.h>

static void write_callback(struct SoundIoOutStream* outstream,
                           int frame_count_min,
                           int frame_count_max);

jungle::core::audio::Engine::OutStream::OutStream(
    jungle::core::audio::Engine* parent_engine,
    float latency_s)
    : latency_s(latency_s)
    , muted(false)
    , parent_engine(parent_engine)
{
	int err;

	outstream = soundio_outstream_create(parent_engine->device);

	outstream->format = SoundIoFormatFloat32NE;
	outstream->write_callback = write_callback;

	outstream->software_latency = latency_s;
	outstream->sample_rate = jungle::core::SampleRateHz;

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

jungle::core::audio::Engine::OutStream::~OutStream()
{
	soundio_outstream_destroy(outstream);
}

void jungle::core::audio::Engine::OutStream::toggle_mute() { muted = !muted; }

bool jungle::core::audio::Engine::OutStream::is_muted() { return muted; }

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
