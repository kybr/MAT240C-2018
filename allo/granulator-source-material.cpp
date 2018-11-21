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
    bool active = false;

    float operator()() {
      // the next sample from the grain is taken from the source buffer
      float f = envelop() * source->get(index());

      // if the index has reached its goal, then deactivate this grain
      if (index.done()) active = false;

      return f;
    }
  };

  // we store a "pool" of grains which may or may not be active at any time
  //
  vector<Grain> grain;

  Granulator() {
    // rather than using new/delete and allocating memory on the fly, we just
    // allocate as many grains as we might need---a fixed number that we think
    // will be enough. we can find this number through trial and error. if
    // too many grains are active, we may take too long in the audio callback
    // and that will cause drop-outs and glitches.
    //
    grain.resize(1000);
  }

  // this might help us tune the size of the grain pool
  //
  int activeGrainCount = 0;

  // gui tweakable parameters
  //
  int whichClip = 0;           // (0, source.size())
  float grainDuration = 0.25;  // in seconds
  float startPosition = 0.25;  // (0, 1)
  float peakPosition = 0.1;    // (0, 1)
  float amplitudePeak = 0.9;   // (0, 1)
  float playbackRate = 0;      // (-1, 1)

  // this oscillator governs the rate at which grains are created
  //
  Edge grainBirth;

  // this method makes a new grain out of a dead / inactive one.
  //
  void recycle(Grain& g) {
    // choose which sound clip this grain pulls from
    g.source = soundClip[whichClip];

    // startTime and endTime are in units of sample
    float startTime = g.source->size * startPosition;
    float endTime = startTime + grainDuration * SAMPLE_RATE;

    // this is actually broken, even though it seems like it works
    float t = pow(2.0, playbackRate) * grainDuration * SAMPLE_RATE;
    startTime -= t / 2;
    endTime += t / 2;

    g.index.set(startTime, endTime, grainDuration);

    // riseTime and fallTime are in units of second
    float riseTime = grainDuration * peakPosition;
    float fallTime = grainDuration - riseTime;
    g.envelop.set(riseTime, fallTime, amplitudePeak);

    // permit this grain to sound!
    g.active = true;
  }

  // make the next sample
  //
  float operator()() {
    // figure out if we should generate (recycle) more grains; then do so.
    //
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

struct MyApp : App {
  bool show_gui = true;
  float background = 0.21;

  Granulator granulator;

  void onCreate() override {
    initIMGUI();

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

    ImGui::SliderInt("Sound Clip", &granulator.whichClip, 0, 8);
    ImGui::SliderFloat("Start Position", &granulator.startPosition, 0, 1);
    ImGui::SliderFloat("Playback Rate", &granulator.playbackRate, -2, 2);

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
