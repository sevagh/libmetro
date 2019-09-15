#ifndef JUNGLE_H
#define JUNGLE_H

#include <functional>
#include <memory>
#include <vector>

namespace jungle {
namespace tempo {

using Func = std::function<void()>;

class Tempo {
 public:
  int bpm;
  int period_us;
  Tempo(int bpm);
  void start();
  void register_func(Func f);

 private:
  std::vector<Func> funcs;
};
};  // namespace tempo

namespace audio {

using Tone = std::vector<double>;

class Engine {
 public:
  Engine();
  ~Engine();
  Tone generate_tone(int duration_us, double pitch_hz);
  void play_tone(Tone tone);

 private:
  struct SoundIo *soundio;
  struct SoundIoDevice *device;
};
};  // namespace audio
};  // namespace jungle

#endif /* JUNGLE_H */
