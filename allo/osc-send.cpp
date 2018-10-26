#include "al/core.hpp"

using namespace al;

struct MyApp : App {
  void onCreate() override {
    //
  }

  void onDraw(Graphics& g) override { g.clear(0); }

  void onMessage(osc::Message& m) override {
    m.print();
    //
    //
  }
  void onSound(AudioIOData& io) override {
    while (io()) {
      float s = 0;
      s *= 0.2;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() {
  MyApp app;
  app.initAudio(44100, 512, 2, 0);
  app.start();
}
