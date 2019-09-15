#include <iostream>
#include "libjungle.h"

int main() {
  std::cout << "init audio engine" << std::endl;
  auto audio_engine = jungle::audio::Engine();

  std::cout << "Generating tone" << std::endl;
  auto tone = audio_engine.generate_tone(100000, 440.0);

  // create a lambda
  auto play_tone_lambda = [&]() { audio_engine.play_tone(tone); };

  std::cout << "init 200bpm tempo ticker" << std::endl;
  auto tempo = jungle::tempo::Tempo(200);  // 200bpm
  tempo.register_func(play_tone_lambda);
  tempo.start();
  // tempo.stop();
  // tempo.reset();
}
