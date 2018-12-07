#include "al/core.hpp"
#include "al/util/imgui/al_Imgui.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

const int N = 44100 * 2;
struct Delay {
  // Circular buffer
  float data[N] = {0};
  int delay_amount = 22050;
  int index = 0;  // wrap this

  float operator()(float f) {
    data[index] = data[index] / 5 + f;  // remmeber
    index = 1 + index;
    if (index >= N) index = 0;
    int ago_index = index - delay_amount;
    if (ago_index < 0) ago_index += N;
    return data[ago_index];
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
    edge.period(0.2);
    line.set(1, 0, 0.3);  // start at value, end at value, time to get there
    // edge.period(0.002);
    // edge.period(0.0000002);
  }

  void onAnimate(double dt) override {
    beginIMGUI_minimal(show_gui);
    navControl().active(!imgui_is_using_input());
  }

  void onDraw(Graphics& g) override {
    static float period = 0.2;
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
