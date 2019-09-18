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

static std::vector<float> generate_sinewave(int duration_us, double pitch_hz);

jungle::audio::Tone::Tone(int duration_us, double pitch_hz) : duration_us(duration_us), pitch_hz(pitch_hz), data(generate_sinewave(duration_us, pitch_hz)) { }

static std::vector<float> generate_sinewave(int duration_us, double pitch_hz) {
  size_t size = duration_us * SAMPLE_RATE_HZ / 1000000.0;
  size_t lut_size = size / 4;

  std::vector<int> lut{};
  float *_tone = (float *)malloc(sizeof(float) * size / 2);

  float delta_phi = pitch_hz * lut_size * 1.0 / SAMPLE_RATE_HZ;
  float phase = 0.0;

  for (int i = 0; i < signed(lut_size); ++i) {
    lut.push_back((int)roundf(0x7FFF * sinf(2.0 * M_PI * i / lut_size)));
  }

  float min = DBL_MAX;
  float max = -DBL_MAX;
  for (int i = 0; i < signed(size / 2); ++i) {
    int val = float(lut[(int)phase]);
    if (val > max) {
      max = val;
    }
    if (val < min) {
      min = val;
    }
    _tone[i] = val;
    phase += delta_phi;
    if (phase >= lut_size) phase -= lut_size;
  }

  return std::vector<float>(_tone, _tone + size / 2);
}
