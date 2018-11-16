#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
#include "al/util/imgui/al_Imgui.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

// this is for version 2:
// https://en.cppreference.com/w/cpp/container/priority_queue
//#include <queue>  // priority_queue
// priority_queue<Grain*> grain;

#include <vector>
using namespace std;

struct Granulator {
  struct Grain {
    bool active = false;      // true if this grain is for real
    Array* source = nullptr;  // where to get sound material
    Line index;    // index into the the source; captures playback rate
    Line envelop;  // we can to better; we need an AttackDecay class
    float amplitude = 1;
    float operator()() { return amplitude * envelop() * source->get(index()); }
  };

  // we should have a set of audio buffers containing sound clips from which we
  // can draw source material for grains.
  //
  vector<Array*> buffer;

  // we need a container to hold our grains
  //
  vector<Grain> grain;

  Granulator() { grain.resize(100); }

  // Properties or parameters of the granulator might be:
  // - density or rate: how often we should make new grains
  // - intensity: the amplitude or loudness of grains
  // - envelop rise time: meh. i'm tire of typing
  //
  // grain parameters may be chosen randomly, but we would like to limit
  // that randomness. we would like access to ways of choosing from several
  // kinds of random distributions: normal, uniform, poisson, etc. so our
  // high level interface is setting the mean and standard deviation and
  // range of these.
  //

  //
  //
  void recycle(Grain& g) {
    // consider this grain as dead; configure all the parameters and make it
    // active. this is where we use our high-level parameters to choose settings
    // at a low level.
  }

  void density(float gps) {
    // possible rename: birthrate, grain emmision density, rate

    // change the rate at which we spew new grains
  }

  // make the next sample
  //
  float operator()() {
    // figure out if we should generate (recycle) more grains; do so.
    //
    // figure out which grains are active. for each active grain, get the next
    // sample; sum all these up and return that sum.
    //
    return 0;
  }
};

void load(Granulator& g, string fileName);

struct MyApp : App {
  bool show_gui = true;
  float background = 0.21;

  Granulator granulator;

  void onCreate() override {
    initIMGUI();
    load(granulator, "0.wav");
    load(granulator, "1.wav");
    load(granulator, "2.wav");
    load(granulator, "3.wav");
    load(granulator, "4.wav");
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
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}

// knows how to load a file into the granulator
//
void load(Granulator& granulator, string fileName) {
  SearchPaths searchPaths;
  searchPaths.addSearchPath("..");

  string filePath = searchPaths.find(fileName).filepath();
  SoundFile soundFile;
  soundFile.path(filePath);
  if (!soundFile.openRead()) {
    cout << "We could not read " << fileName << "!" << endl;
    exit(1);
  }
  if (soundFile.channels() != 1) {
    cout << fileName << " is not a mono file" << endl;
    exit(1);
  }

  Array* a = new Array();
  a->size = soundFile.frames();
  a->data = new float[a->size];
  soundFile.read(a->data, a->size);
  granulator.buffer.push_back(a);

  soundFile.close();

  /*
    cout << a->size << " was size" << endl;
    for (float f = 1500; f < 1800; f += 0.33333) {
      float t = a->get(1533.823);
      // float t = granulator.buffer[3]->get(100.823);
      cout << "t: " << t << endl;
    }
    */
   
}
