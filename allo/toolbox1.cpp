#include "al/core.hpp"

using namespace al;
using namespace std;
const int SAMPLE_RATE = 44100;
const int BLOCK_SIZE = 512;
const int OUTPUT_CHANNELS = 2;
const int INPUT_CHANNELS = 2;

struct Phasor {
  float value = 0, rate = 0;
  float operator()() {
    value += rate;
    value = (value < 0) ? (value + 1) : (value > 1 ? value - 1 : value);
    return value;
  }
  void period(float s) { rate = 1 / (s * SAMPLE_RATE); }
  void frequency(float hz) { rate = hz / SAMPLE_RATE; }
};

struct FloatIndexArray {
  float* data = nullptr;
  unsigned size = 0;

  void resize(unsigned size) {
    this->size = size;
    if (data != nullptr) delete[] data;
    data = new float[size];
    for (unsigned i = 0; i < size; i++) data[i] = 0;
  }

  float operator[](float index) {
    // index could be (-oo, oo)
    float integer;
    float fraction = std::modf(index, &integer);

    return 0;
  }
};

struct Sine {};

struct MyApp : App {
  Phasor phasor;

  void onCreate() override { phasor.frequency(220); }

  void onDraw(Graphics& g) override { g.clear(0.1); }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float s = phasor();
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
