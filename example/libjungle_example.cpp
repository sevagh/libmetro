#include "libjungle.h"

int main() {
  auto audio_engine = jungle::audio::Engine();
  auto tempo = jungle::tempo::Tempo(200);  // 200bpm
  tempo.start();
  // tempo.stop();
  // tempo.reset();
}
