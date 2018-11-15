#include "al/core.hpp"
#include "al/util/imgui/al_Imgui.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

struct MyApp : App {
  Sine sine;
  Saw saw;
  Edge edge;
  Line line;
  Biquad biquad;
  bool show_gui = true;
  float background = 0.21;

  void onCreate() override {
    initIMGUI();
    sine.frequency(440);
    saw.frequency(440);
    edge.period(0.2);
    line.set(1, 0, 0.3);  // start at value, end at value, time to get there
    biquad.lpf(1200, 1);
    // edge.period(0.002);
    // edge.period(0.0000002);
  }

  void onAnimate(double dt) override {
    // pass show_gui for use_input param to turn off interactions
    // when not showing gui
    beginIMGUI_minimal(show_gui);
    navControl().active(!imgui_is_using_input());
  }

  void onDraw(Graphics& g) override {
    g.clear(background);
    ImGui::SliderFloat("grayscale", &background, 0, 1);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    static float period = 0.2;
    ImGui::SliderFloat("Edge Period", &period, 0, 1);
    edge.period(period);

    endIMGUI_minimal(show_gui);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      // this inner code block runs once per sample
      if (edge()) {
        sine.frequency(rnd::uniform(220.0, 880.0));
        line.set(1, 0, 0.4);
        biquad.lpf(rnd::uniform(220.0, 880.0), rnd::uniform(0.01, 2.0));
      }
      // float s = biquad(saw());
      // float s = sine() * line();
      float s = biquad(saw() * line());
      s *= 0.1;
      io.out(0) = s;
      io.out(1) = s;
    }
  }

  void onKeyDown(const Keyboard& k) override {
    if (k.key() == 'g') {
      show_gui = !show_gui;
    }
  }

  void onExit() override { shutdownIMGUI(); }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
