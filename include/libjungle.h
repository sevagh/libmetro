#ifndef JUNGLE_H
#define JUNGLE_H

#include <memory>
#include <vector>

namespace jungle {
namespace tempo {
class Tempo {
 public:
  Tempo(int bpm);
  ~Tempo();  // explicit destructors cause of PIMPL
  void start();
  void stop();
  void reset();

 private:
  class impl;
  std::unique_ptr<impl> pimpl;
};
};  // namespace tempo

namespace audio {

using Tone = std::vector<double>;

class Engine {
 public:
  Engine();
  ~Engine();
  const Tone generate_tone(int duration_ns, double pitch_hz);
  void play_tone(const Tone tone);

 private:
  class impl;
  std::unique_ptr<impl> pimpl;
};
};  // namespace audio
};  // namespace jungle

#endif /* JUNGLE_H */
