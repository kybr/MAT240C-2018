#include "al/core.hpp"
using namespace al;

#include "Gamma/DFT.h"
#include "Gamma/Filter.h"
#include "Gamma/Oscillator.h"
#include "Gamma/SamplePlayer.h"
using namespace gam;

#include "synths.h"
using namespace diy;

SamplePlayer<float, gam::ipl::Linear, phsInc::Loop> player;
STFT stft;

struct MyApp : App {
  Mesh spectrum;

  void onCreate() override {
    spectrum.primitive(Mesh::LINE_STRIP);
    for (unsigned k = 0; k < stft.numBins(); k++) {
      // XXX set the x-axis to be log
      spectrum.vertex(float(k) / stft.numBins(), 0);
    }

    SearchPaths searchPaths;
    searchPaths.addSearchPath("..");
    std::string filePath = searchPaths.find("superstition.wav").filepath();
    player.load(filePath.c_str());
    player.rate(1);
    player.reset();

    // set the viewer back a little
    nav().pos(0, 0, 10);
  }

  void onDraw(Graphics& g) override {
    g.clear(0);        // background: black
    g.color(1);        // draw color: white
    g.draw(spectrum);  // draw the mesh
  }

  void onSound(AudioIOData& io) override {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      float s = player();
      if (stft(s)) {
        // your FFT bin are ready, sir!
        for (unsigned k = 0; k < stft.numBins(); k++) {
          // XXX put y-axis in dB
          spectrum.vertices()[k].y = stft.bin(k).mag();

          // change the spectrum; make it all imaginary
          stft.bin(k).imag() = stft.bin(k).mag();
          stft.bin(k).real() = 0;
        }
      }

      s = stft();
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
