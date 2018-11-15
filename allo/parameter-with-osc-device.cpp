#include "al/core.hpp"
#include "al/util/imgui/al_Imgui.hpp"
#include "al/util/ui/al_ControlGUI.hpp"
#include "al/util/ui/al_Parameter.hpp"
using namespace al;

struct MyApp : App {
  bool show_gui = true;
  float background = 0.21;

  Parameter x{"x", "gravity", 0.0f, "accelerometer"};
  Parameter y{"y", "gravity", 0.0f, "accelerometer"};
  Parameter z{"z", "gravity", 0.0f, "accelerometer"};

  Parameter azimuth{"azimuth", "orientation", 0.0f};
  Parameter pitch{"pitch", "orientation", 0.0f};
  Parameter roll{"roll", "orientation", 0.0f};

  void onCreate() override {
    initIMGUI();
    parameterServer() << x << y << z;
    parameterServer() << azimuth << pitch << roll;
    parameterServer().print();
  }
  void onMessage(osc::Message& m) override {
    //
    // override this and do nothing unless you want to print each message

    // m.print();
  }

  void onAnimate(double dt) override {
    beginIMGUI_minimal(show_gui);
    navControl().active(!imgui_is_using_input());
    //    std::cout << x << std::endl;
    float f = x;
    f -= (int)f;
    background = f;

    //    Vec3f(x, y, z).print();
    //    std::cout << std::endl;
  }

  void onDraw(Graphics& g) override {
    g.clear(background);
    ImGui::SliderFloat("grayscale", &background, 0, 1);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    endIMGUI_minimal(show_gui);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float s = 0;
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
  app.initAudio(44100, 512, 2, 2);
  app.start();
}
