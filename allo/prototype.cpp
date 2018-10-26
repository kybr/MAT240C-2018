#include "al/core.hpp"
using namespace al;
// some constants
const int SAMPLE_RATE = 44100;
const int BLOCK_SIZE = 512;
const int OUTPUT_CHANNELS = 2;
const int INPUT_CHANNELS = 2;

// define new classes (aka struct) here
//

struct MyApp : App {
  // put "class" variables here
  //

  // called once, before any other callbacks
  //
  void onCreate() override {
    // - initialize variables
    // - set default settings
    // - build structures you'll need in onDraw and onSound
    //
  }

  // called once per graphics frame ~30Hz
  //
  void onDraw(Graphics& g) override {
    g.clear(0);
    //
  }

  // called once per audio frame ~86Hz (SAMPLE_RATE / BLOCK_SIZE)
  //
  void onSound(AudioIOData& io) override {
    while (io()) {
      // this inner code block runs once per sample
      float s = 0;
      io.out(0) = s;
      io.out(1) = s;
    }
  }

  // called once per message received; listens on UDP port 9010
  //
  void onMessage(osc::Message& m) override {
    m.print();
    //
    // see "examples/io/midiInApp.cpp" for MIDI
  }
};

// don't change anything below this line
//
int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
