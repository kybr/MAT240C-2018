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

  Mesh m;

  void onCreate() override {
    phase = 0;

    // configure a mesh
    m.primitive(Mesh::Primitive::TRIANGLE_STRIP);
    m.vertex(0, 0);
    m.vertex(0, 1);
    m.vertex(1, 1);

    // move the camera back so we can see
    nav().pos(0, 0, 5);
  }

  void onDraw(Graphics& g) override {
    g.clear(0.1);
    g.color(phase, 1 - phase, phase * phase);

    //
    g.draw(m);
  }

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
