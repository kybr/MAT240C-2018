#include "al/core.hpp"
using namespace al;
// some constants
const int SAMPLE_RATE = 44100;
const int BLOCK_SIZE = 512;
const int OUTPUT_CHANNELS = 2;
const int INPUT_CHANNELS = 2;

// write a program that plays a sine tone

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

struct Edge {
  float phase = 0.0;        // on the interval [0, 1)
  float increment = 0.001;  // led to an low F

  void frequency(float hertz) { increment = hertz / SAMPLE_RATE; }
  void period(float seconds) { frequency(1 / seconds); }

  bool operator()() {
    phase += increment;
    if (phase > 1) {
      phase -= 1;
      return true;
    }
    return false;
  }
};

struct Line {
  float value, target, seconds;
  float increment;

  void set() {
    // slope per sample
    increment = (target - value) / (seconds * SAMPLE_RATE);
  }
  void set(float v, float t, float s) {
    value = v;
    target = t;
    seconds = s;
    set();
  }
  void set(float t, float s) {
    target = t;
    seconds = s;
    set();
  }
  void set(float t) {
    target = t;
    set();
  }

  float operator()() {
    if (value == target) return value;
    value += increment;
    if ((increment < 0) ? (value < target) : (value > target)) value = target;
    return value;
  }
};

/*
// pattern for later
struct Reverb {
  float operator()(float f) { return makeReverbHappenTo(f); }
}
*/

struct Sine : Phasor {
  // how much memory does this cost?
  float data[200000];
  // a float is 4 bytes
  // 200_000 * 4 = 800_000 bytes or 0.8 MB.

  Sine() {
    for (int i = 0; i < 200000; i++) data[i] = sin(2 * M_PI * i / 200000);
  }

  float operator()() {
    float phase = Phasor::operator()();
    return data[int(phase * 200000)];
  }
};

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
        line.set(1, 0, 0.3);
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
