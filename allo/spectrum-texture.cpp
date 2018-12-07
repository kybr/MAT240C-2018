#include "Gamma/DFT.h"
#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#define N (1024)

gam::STFT stft(N, N / 4, 0, gam::HANN, gam::MAG_PHASE);

struct MyApp : App {
  Texture texture;
  std::vector<Colori> pixel;

  //
  Saw saw;
  Edge edge;
  Line amplitude, frequency;

  void onCreate() override {
    // void Texture::create2D(unsigned int width, unsigned int height, int
    // internal, unsigned int format, unsigned int type) {
    texture.create2D(N / 2, N / 2, Texture::RGB8);
    int Nx = texture.width();
    int Ny = texture.height();

    // prepare vector for pixel data
    pixel.resize(Nx * Ny);

    for (int j = 0; j < Ny; ++j) {
      float y = float(j) / (Ny - 1) * 2 - 1;
      for (int i = 0; i < Nx; ++i) {
        float x = float(i) / (Nx - 1) * 2 - 1;
        float px = x * M_PI;
        float py = y * M_PI;
        Color c = RGB(0.12);
        pixel[j * Nx + i] = c;
      }
    }

    printf("%d\n", stft.numBins());

    //
    frequency.set(220, 220, 0.04);
    amplitude.set(0, 0, 0.04);
    edge.period(0.4);

    nav().pos(0, 0, 5);
  }

  void onDraw(Graphics& g) override {
    texture.submit(pixel);
    g.clear(0);
    g.blending(true);
    g.blendModeTrans();
    g.quadViewport(texture);
  }

  int row = 0;
  float min = 999999999, max = -9999999999;
  void onSound(AudioIOData& io) override {
    while (io()) {
      //
      if (edge()) {
        amplitude.set(1, 0, 0.4);
        frequency.set(mtof(rnd::uniform(16, 111)), 0.1);
      }
      saw.frequency(frequency());
      float s = saw() * amplitude();

      if (stft(s)) {
        int R = stft.numBins() - 1;
        for (unsigned k = 0; k < stft.numBins() - 1; k++) {
          float f = stft.bin(k)[0];
          if (f > max) max = f;
          if (f < min) min = f;
          pixel[row * (N / 2) + k].r = f * 255;
          pixel[row * (N / 2) + k].b = f * 255;
          pixel[row * (N / 2) + k].g = f * 255;
        }
        row++;
        if (row >= N / 2) row = 0;
      }
      io.out(0) = s * 0.1;
      io.out(1) = s * 0.1;
    }
    // printf("min:%f max:%f\n", min, max);
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
