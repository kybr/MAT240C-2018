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
    bool active = false;
    Array* source = nullptr;
    Line index;
    AttackDecay envelop;

    float operator()() {
      float f = envelop() * source->get(index());
      if (index.done()) {
        // if the index has reached its goal, then deactivate this grain
        active = false;
      }
      return f;
    }
  };

  vector<Grain> grain;  // stores grains, which are maybe inactive

  Granulator() {
    // rather than using new/delete and allocating memory on the fly, we just
    // allocate as many grains as we might need---a fixed number.
    //
    grain.resize(1000);
  }

  vector<Array*> soundClip;

  // gui tweakable parameters
  //
  Edge grainBirth;
  int whichClip = 0;
  float grainDuration = 0.25;  // in seconds
  float startPosition = 0.25;  // (0, 1)
  float peakPosition = 0.1;    // (0, 1)
  float amplitudePeak = 0.9;

  // this method makes a new grain out of a dead / inactive one.
  //
  void recycle(Grain& g) {
    // choose which sound clip this grain pulls from
    g.source = soundClip[whichClip];

    // startTime and endTime are in units of sample
    float startTime = g.source->size * startPosition;
    float endTime = startTime + grainDuration * SAMPLE_RATE;
    g.index.set(startTime, endTime, grainDuration);

    // riseTime and fallTime are in units of second
    float riseTime = grainDuration * peakPosition;
    float fallTime = grainDuration - riseTime;
    g.envelop.set(riseTime, fallTime, amplitudePeak);

    // permit this grain to sound!
    g.active = true;
  }

  int activeGrainCount = 0;

  // make the next sample
  //
  float operator()() {
    // figure out if we should generate (recycle) more grains; do so.
    //
    // grainBirth.frequency(noise());
    if (grainBirth()) {
      for (Grain& g : grain)
        if (!g.active) {
          recycle(g);
          break;
        }
    }

    // figure out which grains are active. for each active grain, get the next
    // sample; sum all these up and return that sum.
    //
    float f = 0;
    activeGrainCount = 0;
    for (Grain& g : grain)
      if (g.active) {
        activeGrainCount++;
        f += g();
      }
    return f;
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
    ImGui::Text("Active Grains: %3d", granulator.activeGrainCount);
    ImGui::SliderFloat("Background", &background, 0, 1);

    ImGui::SliderInt("Sound Clip", &granulator.whichClip, 0, 4);
    ImGui::SliderFloat("Start Position", &granulator.startPosition, 0, 1);

    static float volume = -7;
    ImGui::SliderFloat("Loudness", &volume, -42, 0);
    granulator.amplitudePeak = dbtoa(volume);

    ImGui::SliderFloat("Envelop Parameter", &granulator.peakPosition, 0, 1);
    ImGui::SliderFloat("Grain Duration", &granulator.grainDuration, 0.001, 0.5);

    static float midi = 10;
    ImGui::SliderFloat("Birth Frequency", &midi, -16, 85);
    granulator.grainBirth.frequency(mtof(midi));

    endIMGUI_minimal(show_gui);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float s = granulator();
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
  granulator.soundClip.push_back(a);

  soundFile.close();

  /*
    cout << a->size << " was size" << endl;
    for (float f = 1500; f < 1800; f += 0.33333) {
      float t = a->get(1533.823);
      // float t = granulator.soundClip[3]->get(100.823);
      cout << "t: " << t << endl;
    }
    */
}
