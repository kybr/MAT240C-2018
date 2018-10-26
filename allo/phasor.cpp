#include "al/core.hpp"

using namespace al;

#define SAMPLE_RATE (44100)

struct LinearInterpolationArray {
  float* data;
  int length;
  LinearInterpolationArray() {
    length = 20000;
    data = new float[length];
    for (int i = 0; i < length; i++) data[i] = 0;
  }
  // possible index: 0.0, -100.1, 10000000.01,
  float getFloatAt(float index) {
    // fix index: modulo wrt length

    while (index > length) index -= length;
    while (index < 0) index += length;
    int left = int(index);
    int right = 1 + left;
    float t = index - left;
    return (data[left] * (1 - t) + data[right] * t);
    //
  }
};

struct Line {  // Fade
};

struct Phasor {
  float phase = 0.0;        // on the interval [0, 1)
  float increment = 0.001;  // led to an low F

  void frequency(float hertz) { increment = hertz / SAMPLE_RATE; }

  float operator()() {
    phase += increment;
    if (phase > 1) phase -= 1;
    if (phase < 0) phase += 1;
    return phase;
  }
};

struct Sine : Phasor {
  float data[200000];
  Sine() {
    for (int i = 0; i < 200000; i++) data[i] = sin(2 * M_PI * i / 200000);
  }

  float operator()() {
    float phase = Phasor::operator()();
    return data[int(phase * 200000)];
  }
};

struct MyApp : App {
  Phasor p;
  Sine sine;
  LinearInterpolationArray array;

  void onCreate() override {
    p.frequency(440);
    sine.frequency(440);
    array.data[0] = 1.0f;
  }

  void onDraw(Graphics& g) override {
    g.clear(0);

    // draw the phasor
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float s = sine();
      // float s = sin(p() * 2 * M_PI);  // Generate next sine wave sample
      s *= 0.2;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, 512, 2, 0);
  app.start();
}
