#ifndef JUNGLE_H
#define JUNGLE_H

#include <functional>
#include <memory>
#include <vector>

namespace jungle {

void eventloop();

namespace tempo {

using Func = std::function<void()>;

class Tempo {
 public:
  int bpm;
  int period_us;
  Tempo(int bpm);
  void start();
  void register_func_cycle(std::vector<Func> cycle);

 private:
  std::vector<std::vector<Func>> func_cycles;
  std::vector<size_t> func_cycle_indices;
};
};  // namespace tempo

namespace audio {

class Tone {
 public:
  int duration_us;
  double pitch_hz;
  std::vector<float> data;
  Tone(int duration_us, double pitch_hz);
};

class Engine;

class Stream {
  friend class Engine;

 public:
  Stream(Engine &engine, int latency_us);
  ~Stream();
  void play_tone(Tone &tone);
  void stop();

 private:
  Engine &parent;
  struct SoundIoOutStream *outstream;
  struct SoundIoRingBuffer *ringbuf;
};

class Engine {
  friend class Stream;

 public:
  Engine();
  ~Engine();

 private:
  struct SoundIo *soundio;
  struct SoundIoDevice *device;
};
};  // namespace audio
};  // namespace jungle

#endif /* JUNGLE_H */
