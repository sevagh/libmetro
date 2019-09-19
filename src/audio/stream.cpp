#include <soundio/soundio.h>
#include <stdio.h>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <future>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "libjungle.h"

static void write_callback(struct SoundIoOutStream *outstream,
                           int frame_count_min, int frame_count_max);

jungle::audio::Stream::Stream(jungle::audio::Engine &engine, int latency_us) : parent(engine) {
  int err;

  outstream = soundio_outstream_create(engine.device);

  outstream->format = SoundIoFormatFloat32NE;
  outstream->write_callback = write_callback;

  outstream->software_latency = latency_us/1000000.0;
  outstream->sample_rate = jungle::SAMPLE_RATE_HZ;

  if ((err = soundio_outstream_open(outstream)))
    throw std::runtime_error(std::string("unable to open device: ") +
                             soundio_strerror(err));

  if ((err = soundio_outstream_start(outstream)))
    throw std::runtime_error(std::string("unable to start device: ") +
                             soundio_strerror(err));
}

jungle::audio::Stream::~Stream() { soundio_outstream_destroy(outstream); }

void jungle::audio::Stream::play_tone(jungle::audio::Tone &tone) {
  outstream->userdata = reinterpret_cast<void*>(&tone);
}

static void write_callback(struct SoundIoOutStream *outstream,
                           __attribute__((unused)) int frame_count_min,
                           int frame_count_max) {
  const struct SoundIoChannelLayout *layout = &outstream->layout;
  struct SoundIoChannelArea *areas;

  if (outstream->userdata == nullptr) return;

  auto tone = reinterpret_cast<jungle::audio::Tone*>(outstream->userdata);

  size_t frames_left = frame_count_min;
  if (frames_left == 0)
    frames_left = frame_count_max;

  int err;

  while (frames_left > 0) {
    int frame_count = std::min(tone->size(), frames_left);

    if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count)))
      throw std::runtime_error(soundio_strerror(err));

    if (!frame_count) break;

    for (int frame = 0; frame < frame_count; ++frame) {
      float sample = (*tone)[frame];
      for (int channel = 0; channel < layout->channel_count; ++channel) {
        float *ptr =
            (float *)(areas[channel].ptr + areas[channel].step * frame);
        *ptr = sample;
      }
    }

    if ((err = soundio_outstream_end_write(outstream)))
      throw std::runtime_error(soundio_strerror(err));
    else
	    outstream->userdata = nullptr;

    frames_left -= frame_count;
  }
}
