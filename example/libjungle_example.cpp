#include <iostream>
#include "libjungle.h"

int main() {
  std::cout << "init audio engine" << std::endl;
  auto audio_engine = jungle::audio::Engine();

  int bpm = 100;
  std::cout << "init " << bpm << "bpm tempo ticker" << std::endl;

  auto tempo = jungle::tempo::Tempo(bpm);

  std::cout << "Generating tones" << std::endl;

  auto downbeat = audio_engine.generate_tone(tempo.period_us / 2, 540.0);
  auto beat = audio_engine.generate_tone(tempo.period_us / 2, 350.0);

  // create a cycle of lambdas
  std::vector<jungle::tempo::Func> beat22 = {
      [&]() { audio_engine.play_tone(downbeat); },
      [&]() { audio_engine.play_tone(beat); },
  };

  tempo.register_func_cycle(beat22);
  tempo.start();

  jungle::eventloop();
}
