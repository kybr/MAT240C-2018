#ifndef __240C_SYNTHS__
#define __240C_SYNTHS__

namespace diy {

const int SAMPLE_RATE = 44100;
const int BLOCK_SIZE = 512;
const int OUTPUT_CHANNELS = 2;
const int INPUT_CHANNELS = 2;

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
    if (value != target) {
      value += increment;
      if ((increment < 0) ? (value < target) : (value > target)) value = target;
    }
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

}  // namespace diy
#endif  // __240C_SYNTHS__
