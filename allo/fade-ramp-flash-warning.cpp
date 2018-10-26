#include "al/core.hpp"
using namespace al;
using namespace std;
// some constants
const int SAMPLE_RATE = 44100;
const int BLOCK_SIZE = 512;
const int OUTPUT_CHANNELS = 2;
const int INPUT_CHANNELS = 2;

struct MyApp : App {
  float phase, increment;

  void onCreate() override { phase = 0; }

  void onDraw(Graphics& g) override { g.clear(phase); }

  void onSound(AudioIOData& io) override {
    while (io()) {
      phase += increment;
      if (phase > 1) phase -= 1;
      float s = phase;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
  void onMessage(osc::Message& m) override {
    m.print();
    // cout << m.addressPattern() << endl;
    m >> increment;
  }
};

// don't change anything below this line
//
int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
