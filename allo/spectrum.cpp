#include "Gamma/DFT.h"
#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

gam::STFT stft;

struct MyApp : App {
  Sine sine;
  Edge edge;
  Line line;

  Mesh spectrum;

  void onCreate() override {
    sine.frequency(440);
    edge.period(1.0);
    line.set(rnd::uniform(55.0, 1760.0), rnd::uniform(0.3, 2.1));

    spectrum.primitive(Mesh::LINE_STRIP);
    for (unsigned k = 0; k < stft.numBins(); k++) {
      // XXX set the x-axis to be log
      spectrum.vertex(float(k) / stft.numBins(), 0);
    }

    // set the viewer back a little
    nav().pos(0, 0, 10);
  }

  void onDraw(Graphics& g) override {
    g.clear(0);        // background: black
    g.color(1);        // draw color: white
    g.draw(spectrum);  // draw the mesh
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      if (edge()) {
        line.set(rnd::uniform(55.0, 1760.0), rnd::uniform(0.3, 2.1));
        // set a new period for edge
      }
      sine.frequency(line());
      float s = sine();

      if (stft(s)) {
        // your FFT bin are ready, sir!
        for (unsigned k = 0; k < stft.numBins(); k++) {
          // XXX put y-axis in dB
          spectrum.vertices()[k].y = stft.bin(k).mag();
        }
      }

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
