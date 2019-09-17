#include <soundio/soundio.h>
#include <stdio.h>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <future>
#include <iostream>
#include <memory>
#include <vector>
#include "libjungle.h"

static const double SAMPLE_RATE_HZ = 48000.0;

static std::vector<double> generate_sinewave(int duration_us, double pitch_hz);

static void write_callback(struct SoundIoOutStream *outstream,
                           int frame_count_min, int frame_count_max);

static std::future<void> eventloop_handle;

jungle::audio::Engine::Engine() {
  int err;

  soundio = soundio_create();
  if (!soundio) throw std::runtime_error("out of memory");

  if ((err = soundio_connect(soundio)))
    throw std::runtime_error(std::string("error connecting: ") +
                             soundio_strerror(err));

  soundio_flush_events(soundio);

  int default_out_device_index = soundio_default_output_device_index(soundio);
  if (default_out_device_index < 0)
    throw std::runtime_error("no output device found");

  device = soundio_get_output_device(soundio, default_out_device_index);
  if (!device) throw std::runtime_error("out of memory");

  auto blocking_eventloop = [&]() {
    for (;;) {
      std::cout << "SOUNDIO WAIT" << std::endl;
      soundio_wait_events(soundio);
    }
  };

  eventloop_handle = std::async(std::launch::async, blocking_eventloop);

  std::cout << "Using default output device: " << device->name << std::endl;

  outstream = soundio_outstream_create(device);
  outstream->format = SoundIoFormatFloat32NE;
  outstream->write_callback = write_callback;

  if ((err = soundio_outstream_open(outstream)))
    throw std::runtime_error(std::string("unable to open device: ") +
                             soundio_strerror(err));
}

jungle::audio::Engine::~Engine() {
  soundio_outstream_destroy(outstream);
  soundio_device_unref(device);
  soundio_destroy(soundio);
}

jungle::audio::Tone jungle::audio::Engine::generate_tone(int duration_us,
                                                         double pitch_hz) {
  return generate_sinewave(duration_us, pitch_hz);
}

void jungle::audio::Engine::play_tone(jungle::audio::Tone &tone) {
  int err;

  outstream->userdata = reinterpret_cast<void *>(&tone);

  if ((err = soundio_outstream_start(outstream)))
    throw std::runtime_error(std::string("unable to start device: ") +
                             soundio_strerror(err));
}

static std::vector<double> generate_sinewave(int duration_us, double pitch_hz) {
  size_t size = duration_us * SAMPLE_RATE_HZ / 1000000.0;
  size_t lut_size = size / 4;

  std::vector<int> lut{};
  double *_tone_single_channel = (double *)malloc(sizeof(double) * size / 2);

  double delta_phi = pitch_hz * lut_size * 1.0 / SAMPLE_RATE_HZ;
  double phase = 0.0;

  for (int i = 0; i < signed(lut_size); ++i) {
    lut.push_back((int)roundf(0x7FFF * sinf(2.0 * M_PI * i / lut_size)));
  }

  double min = DBL_MAX;
  double max = -DBL_MAX;
  for (int i = 0; i < signed(size / 2); ++i) {
    int val = double(lut[(int)phase]);
    if (val > max) {
      max = val;
    }
    if (val < min) {
      min = val;
    }
    _tone_single_channel[i] = val;
    phase += delta_phi;
    if (phase >= lut_size) phase -= lut_size;
  }

  std::vector<double> tone_single_channel(_tone_single_channel,
                                          _tone_single_channel + size / 2);

  return tone_single_channel;
}

static void write_callback(struct SoundIoOutStream *outstream,
                           __attribute__((unused)) int frame_count_min,
                           int frame_count_max) {
  std::cout << "write_callback is being called all over again!" << std::endl
            << std::endl;
  const struct SoundIoChannelLayout *layout = &outstream->layout;
  struct SoundIoChannelArea *areas;

  if (outstream->userdata == nullptr) return;

  auto tone = reinterpret_cast<jungle::audio::Tone *>(outstream->userdata);

  int frames_left = std::min(frame_count_max, (int)tone->size());
  int err;

  std::cout << "FRAMES LEFT: " << frames_left << std::endl;

  while (frames_left > 0) {
    int frame_count = frames_left;

    if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count)))
      throw std::runtime_error(soundio_strerror(err));
    else
      std::cout << "BEGIN WRITE!" << std::endl;

    if (!frame_count) break;

    for (int frame = 0; frame < frame_count; frame += 1) {
      for (int channel = 0; channel < layout->channel_count; channel += 1) {
        float *ptr =
            (float *)(areas[channel].ptr + areas[channel].step * frame);
        *ptr = (*tone)[frame];
      }
    }

    if ((err = soundio_outstream_end_write(outstream)))
      throw std::runtime_error(soundio_strerror(err));
    else
      std::cout << "END WRITE!" << std::endl;

    frames_left -= frame_count;
  }
}
