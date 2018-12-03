#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
#include "al/util/imgui/al_Imgui.hpp"
#include "al/util/ui/al_ControlGUI.hpp"
#include "al/util/ui/al_Parameter.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

// this is for version 2:
// https://en.cppreference.com/w/cpp/container/priority_queue
//#include <queue>  // priority_queue
// priority_queue<Grain*> grain;

#include <set>
#include <stack>
#include <vector>
using namespace std;

struct FloatPair {
  float l, r;
};

struct Granulator {
  // knows how to load a file into the granulator
  //
  void load(string fileName) {
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
    this->soundClip.push_back(a);

    soundFile.close();
  }

  // we keep a set of sound clips in memory so grains may use them
  //
  vector<Array*> soundClip;

  // we define a Grain...
  //
  struct Grain {
    Array* source = nullptr;
    Line index;  // this is like a tape play head that scrubs through the source
    AttackDecay envelop;  // new class handles the fade in/out and amplitude
    float pan;

    float operator()() {
      // the next sample from the grain is taken from the source buffer
      return envelop() * source->get(index());
    }
  };

  template <typename T>
  class Bag {
    set<T*> active;
    forward_list<T*> remove, inactive;

   public:
    // get any element
    Grain* get_any_inactive() {
      if (inactive.empty()) return nullptr;
      active.insert(inactive.top());
      inactive.pop();
      return t;
    }
    void schedule_for_deactivation(T* t) { remove.push_front(t); }
    void execute_deactivation() {
      for (auto e : remove) {
        active.erase(e);
        inactive.push_front(e);
      }
    }
  };

  // we store a "pool" of grains which may or may not be active at any time
  //
  vector<Grain> grain;
  set<Grain*> active;
  stack<Grain*> inactive;
  Bag<Grain*> bag;

  Granulator() {
    // rather than using new/delete and allocating memory on the fly, we just
    // allocate as many grains as we might need---a fixed number that we think
    // will be enough. we can find this number through trial and error. if
    // too many grains are active, we may take too long in the audio callback
    // and that will cause drop-outs and glitches.
    //
    grain.resize(1000);
    for (int i = 0; i < grain.size(); i++) inactive.push(&grain[i]);
  }

  // this might help us tune the size of the grain pool
  //
  int activeGrainCount = 0;

  // gui tweakable parameters
  //
  ParameterInt whichClip{"/clip", "", 0, "", 0, 8};
  Parameter grainDuration{"/duration", "", 0.25, "", 0.001, 1.0};
  Parameter startPosition{"/position", "", 0.25, "", 0.0, 1.0};
  Parameter peakPosition{"/envelope", "", 0.1, "", 0.0, 1.0};
  Parameter amplitudePeak{"/amplitude", "", 0.707, "", 0.0, 1.0};
  Parameter panPosition{"/pan", "", 0.5, "", 0.0, 1.0};
  Parameter playbackRate{"/playback", "", 0.0, "", -1.0, 1.0};
  Parameter birthRate{"/frequency", "", 55, "", 0, 200};

  // this oscillator governs the rate at which grains are created
  //
  Edge grainBirth;

  // this method makes a new grain out of a dead / inactive one.
  //
  void recycle(Grain* g) {
    // choose which sound clip this grain pulls from
    g->source = soundClip[whichClip];

    // startTime and endTime are in units of sample
    float startTime = g->source->size * startPosition;
    float endTime =
        startTime + grainDuration * SAMPLE_RATE * powf(2.0, playbackRate);

    g->index.set(startTime, endTime, grainDuration);

    // riseTime and fallTime are in units of second
    float riseTime = grainDuration * peakPosition;
    float fallTime = grainDuration - riseTime;
    g->envelop.set(riseTime, fallTime, amplitudePeak);

    g->pan = panPosition;
  }

  // make the next sample
  //
  FloatPair operator()() {
    // figure out if we should generate (recycle) more grains; then do so.
    //
    grainBirth.frequency(birthRate);
    if (grainBirth()) {
      if (!inactive.empty()) {
        Grain* g = inactive.top();
        inactive.pop();
        recycle(g);
        active.insert(g);
      }
    }

    // figure out which grains are active. for each active grain, get the next
    // sample; sum all these up and return that sum.
    //
    float left = 0, right = 0;
    forward_list<Grain*> remove;
    for (auto g : active) {
      float f = g->operator()();
      if (g->index.done()) remove.push_front(g);
      left += f * (1 - g->pan);
      right += f * g->pan;
    }

    for (auto e : remove) {
      active.erase(e);
      inactive.push(e);
    }

    return {left, right};
  }
};

struct MyApp : App {
  float background = 0.21;

  Granulator granulator;
  ControlGUI gui;
  PresetHandler presetHandler{"GranulatorPresets"};
  PresetServer presetServer{"0.0.0.0", 9011};

  void onCreate() override {
    // load sound files into the
    granulator.load("0.wav");
    granulator.load("1.wav");
    granulator.load("2.wav");
    granulator.load("3.wav");
    granulator.load("4.wav");
    granulator.load("5.wav");
    granulator.load("6.wav");
    granulator.load("7.wav");
    granulator.load("8.wav");

    gui.init();
    gui << granulator.whichClip << granulator.grainDuration
        << granulator.startPosition << granulator.peakPosition
        << granulator.amplitudePeak << granulator.panPosition
        << granulator.playbackRate << granulator.birthRate;

    parameterServer() << granulator.whichClip << granulator.grainDuration
                      << granulator.startPosition << granulator.peakPosition
                      << granulator.amplitudePeak << granulator.panPosition
                      << granulator.playbackRate << granulator.birthRate;
    parameterServer().print();
  }

  void onAnimate(double dt) override {
    navControl().active(!gui.usingInput());
    //
  }

  void onDraw(Graphics& g) override {
    g.clear(background);
    gui.draw(g);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      FloatPair p = granulator();
      io.out(0) = p.l;
      io.out(1) = p.r;
    }
  }

  void onExit() override { shutdownIMGUI(); }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
