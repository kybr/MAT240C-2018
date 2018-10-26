#include "al/core.hpp"
using namespace al;
const int SAMPLE_RATE = 44100;
const int BLOCK_SIZE = 512;
const int OUTPUT_CHANNELS = 2;
const int INPUT_CHANNELS = 2;

struct MyApp : App {
  Mesh waveform;
  unsigned cursor = 0;

  void onCreate() override {
    waveform.primitive(Mesh::LINE_STRIP);
    for (int i = 0; i < 10000; i++) waveform.vertex(i / 10000.0);
  }

  void onAnimate(double dt) override {
    // set the nav back a little
    nav().pos(0.65, 0.0, 2.375);
    nav().quat(Quatd(1.000000, 0.000000, 0.000000, 0.000000));
  }

  void onDraw(Graphics& g) override {
    g.clear(0);
    g.color(1, 0.2, 0.1);
    g.draw(waveform);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      // "record" input samples
      waveform.vertices()[cursor].y = io.in(0) + io.in(1);
      cursor++;
      if (cursor == waveform.vertices().size()) cursor = 0;

      float s = 0;
      io.out(0) = s;
      io.out(1) = s;
    }
  }

  void onMessage(osc::Message& m) override { m.print(); }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
