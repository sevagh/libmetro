#include "libjungle.h"
#include <iostream>

int main() {
  std::cout << "init audio engine" << std::endl;
  auto audio_engine = jungle::audio::Engine();

  std::cout << "Generating tone" << std::endl;
  auto tone = audio_engine.generate_tone(100000, 440.0);
  audio_engine.play_tone(tone);

  std::cout << "init 200bpm tempo ticker" << std::endl;
  auto tempo = jungle::tempo::Tempo(200);  // 200bpm
  tempo.start();
  // tempo.stop();
  // tempo.reset();
}
