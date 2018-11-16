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

// a granulator makes lots of grains. Grain will be class as well as Granulator.
//
struct Granulator {
  // each particular grain is born with a set of parameters that don't
  // change in its lifetime. these parameters might have been randomly chosen
  // using some high-level interface, but at a low level, each grain has to have
  // a concrete set of parameters that govern its playback.
  //
  // for each parameter, we should think :
  // - what are the units, if any, of the parameter?
  // - what is the appropriate range of values?
  // - does any parameter depend on this parameter?
  // - what parameters depend on this parameter?
  //
  struct Grain {
    // where does the sound material come from? it comes from a particular
    // region of a particular buffer.
    //
    float* sourceBuffer;     // has samples
    float positionInSource;  // seconds? samples? normalized (0.0, 1.0)?
    float length;            // amount of source material

    // how should this grain be played back? when should we play the grain? at
    // what intensity? with what pitch? what are a few parameters we can
    // tweak?
    //
    bool playbackForward;  // false means play in reverse
    float playbackRate;    // range: (-1, 1) equation: pow(2, [-1, 1]) * sign

    // our envelope scheme (Marc's Triangle) only takes two parameters:
    float envelopRiseTime;       // range: (0, 1)
    float envelopPeakAmplitude;  // range: (0, 1)
    // int whichEnvelopeIndex;

    float whenItShouldPlay;  // seconds (relative or absolute)

    // what else? should we put more here?
    bool active;  // true if this grain is for real
  };

  // we should have a set of audio buffers containing sound clips from which we
  // can draw source material for grains.
  //
  vector<Array> buffer;

  // we need a container to hold our grains
  //
  vector<Grain> grain;

  // Properties or parameters of the granulator might be:
  // - density or rate: how often we should make new grains
  // - intensity: the amplitude or loudness of grains
  // - envelop rise time: meh. i'm tire of typing
  //
  // grain parameters may be chosen randomly, but we would like to limit that
  // randomness. we would like access to ways of choosing from several kinds of
  // random distributions: normal, uniform, poisson, etc. so our high level
  // interface is setting the mean and standard deviation and range of these.
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
    load(granulator, "superstition.wav");
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
    cout << " fail!" << endl;
    exit(1);
  }
  if (soundFile.channels() != 1) {
    cout << " fail!" << endl;
    exit(1);
  }
  granulator.buffer.push_back(Array());
  granulator.buffer.back().resize(soundFile.frames());
  soundFile.write(granulator.buffer.back().data, soundFile.frames());
}