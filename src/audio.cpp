#include <soundio/soundio.h>
#include <stdio.h>
#include <cfloat>
#include <cmath>
#include <future>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include "libjungle.h"

static const double SAMPLE_RATE_HZ = 48000.0;

static std::vector<double> generate_sinewave(int duration_us,
                                             double pitch_hz);

static void write_callback(struct SoundIoOutStream *outstream,
                           int frame_count_min, int frame_count_max);

static std::future<void> eventloop_handle;

class jungle::audio::Engine::impl {
 public:
  struct SoundIo *soundio;
  struct SoundIoDevice *device;

  impl() {
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

    eventloop_handle = std::async(std::launch::async, &impl::blocking_eventloop, this);

    std::cout << "Using default output device: " << device->name << std::endl;
  }

  void blocking_eventloop() {
    for(;;)
  	soundio_wait_events(soundio);
  }

  void play_tone(jungle::audio::Tone t) {
    std::cout << "playing a tone with size: " << t.size() << std::endl;

    int err;

    struct SoundIoOutStream *outstream = soundio_outstream_create(device);
    outstream->format = SoundIoFormatFloat32NE;
    outstream->userdata = reinterpret_cast<void *>(&t);
    outstream->write_callback = write_callback;

    if ((err = soundio_outstream_open(outstream)))
      throw std::runtime_error(std::string("unable to open device: ") +
                               soundio_strerror(err));

    if ((err = soundio_outstream_start(outstream)))
      throw std::runtime_error(std::string("unable to start device: ") +
                               soundio_strerror(err));

    soundio_outstream_destroy(outstream);
  };

  jungle::audio::Tone generate_tone(int duration_us, double pitch_hz) {
    return generate_sinewave(duration_us, pitch_hz);
  }

  ~impl() {
    soundio_device_unref(device);
    soundio_destroy(soundio);
  };
};

jungle::audio::Engine::Engine() : pimpl(std::make_unique<impl>(impl())) {}
 
jungle::audio::Engine::~Engine() = default;

jungle::audio::Tone jungle::audio::Engine::generate_tone(
    int duration_us, double pitch_hz) {
  return this->pimpl->generate_tone(duration_us, pitch_hz);
}

void jungle::audio::Engine::play_tone(jungle::audio::Tone tone) {
  this->pimpl->play_tone(tone);
}

static std::vector<double> generate_sinewave(int duration_us,
                                             double pitch_hz) {
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

static float seconds_offset = 0.0f;

static void write_callback(struct SoundIoOutStream *outstream,
                           __attribute__((unused)) int frame_count_min,
                           int frame_count_max) {
  const struct SoundIoChannelLayout *layout = &outstream->layout;
  float float_sample_rate = outstream->sample_rate;
  float seconds_per_frame = 1.0f / float_sample_rate;
  struct SoundIoChannelArea *areas;

  if (outstream->userdata == nullptr)
	  return;

  auto tone = reinterpret_cast<jungle::audio::Tone &>(outstream->userdata);

  int frames_left = std::min(frame_count_max, (int)tone.size());
  int err;

  while (frames_left > 0) {
    int frame_count = frames_left;

    if ((err =
             soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
      fprintf(stderr, "%s\n", soundio_strerror(err));
      exit(1);
    }

    if (!frame_count) break;

    for (int frame = 0; frame < frame_count; frame += 1) {
      for (int channel = 0; channel < layout->channel_count; channel += 1) {
        float *ptr =
            (float *)(areas[channel].ptr + areas[channel].step * frame);
        *ptr = tone[frame];
      }
    }
    seconds_offset =
        fmodf(seconds_offset + seconds_per_frame * frame_count, 1.0f);

    if ((err = soundio_outstream_end_write(outstream))) {
      fprintf(stderr, "%s\n", soundio_strerror(err));
      exit(1);
    }

    frames_left -= frame_count;
  }
}
