#include "al/core.hpp"
#include "al/util/imgui/al_Imgui.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

struct Delay : Array {
  float delay;
  unsigned next;
  Delay(float capacity = 2) {
    resize(ceil(capacity * SAMPLE_RATE));
    next = 0;
  }

  void period(float seconds) { delay = seconds * SAMPLE_RATE; }
  void frequency(float hertz) { period(1 / hertz); }

  float operator()(float sample) {
    float index = next - delay;
    if (index < 0) index += size;
    float returnValue = get(index);
    data[next] = sample;
    next++;
    if (next >= size) next = 0;
    return returnValue;
  }
};

struct MyApp : App {
  Sine sine;
  Edge edge;
  Line line;
  bool show_gui = true;
  float grayscale = 0.21;

  Delay delay;

  void onCreate() override {
    initIMGUI();
    sine.frequency(440);
    edge.period(1.0);
    line.set(1, 0, 0.3);  // start at value, end at value, time to get there
    delay.period(0.5);
    // edge.period(0.002);
    // edge.period(0.0000002);
  }

  void onAnimate(double dt) override {
    beginIMGUI_minimal(show_gui);
    navControl().active(!imgui_is_using_input());
  }

  void onDraw(Graphics& g) override {
    static float period = 1.0;
    ImGui::SliderFloat("Period", &period, 0.01, 2);
    edge.period(period);

    ImGui::SliderFloat("Background", &grayscale, 0, 1);
    g.clear(grayscale);
    endIMGUI_minimal(show_gui);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      // this inner code block runs once per sample
      if (edge()) {
        sine.frequency(rnd::uniform(220.0, 880.0));
        line.set(1, 0, 0.4);
      }
      float s = sine() * line();
      s /= 2;
      s += delay(s) / 2;
      s *= 0.707;

      io.out(0) = s;
      io.out(1) = s;
    }
  }

  void onExit() override { shutdownIMGUI(); }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
