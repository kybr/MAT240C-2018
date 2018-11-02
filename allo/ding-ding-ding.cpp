#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

struct MyApp : App {
  Sine sine;
  Edge edge;
  Line line;

  void onCreate() override {
    sine.frequency(440);
    edge.period(0.2);
    line.set(1, 0, 0.3);  // start at value, end at value, time to get there
    // edge.period(0.002);
    // edge.period(0.0000002);
  }

  void onDraw(Graphics& g) override { g.clear(0); }

  void onSound(AudioIOData& io) override {
    while (io()) {
      // this inner code block runs once per sample
      if (edge()) {
        sine.frequency(rnd::uniform(220.0, 880.0));
        line.set(1, 0, 0.4);
      }
      float s = sine() * line();
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
