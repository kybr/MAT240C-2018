#include "al/core.hpp"
#include "al/util/imgui/al_Imgui.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

struct MyApp : App {
  Phasor osc1, osc2;
  Line offset;
  bool show_gui = true;
  float background = 0.21;

  void onCreate() override {
    initIMGUI();
    osc1.frequency(220);
    osc2.frequency(220);
  }

  void onAnimate(double dt) override {
    // pass show_gui for use_input param to turn off interactions
    // when not showing gui
    beginIMGUI_minimal(show_gui);
    navControl().active(!imgui_is_using_input());
  }

  int operation = 0;

  // 30~60 Hz
  void onDraw(Graphics& g) override {
    g.clear(background);
    ImGui::SliderFloat("grayscale", &background, 0, 1);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    static float offset_target = 0.2;
    ImGui::SliderFloat("Phase Offset", &offset_target, 0, 1);
    offset.set(offset_target, 0.05);

    ImGui::SliderInt("Operation (+|-|*)", &operation, 0, 2);

    // TODO: would love to SEE this waveform, graphically

    endIMGUI_minimal(show_gui);
  }

  // 86 Hz
  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = osc1.phase + offset();
      if (f > 1) f -= 1;
      osc2.phase = f;

      float s = 0;
      switch (operation) {
        default:
        case 0:
          s = osc1() + osc2();
          break;
        case 1:
          s = osc1() - osc2();
          break;
        case 2:
          s = osc1() * osc2();
          break;
      }
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
