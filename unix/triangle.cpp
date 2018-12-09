#include "everything.h"

// XXX this is broken; it does not process arguments... yet

int main(int argc, char* argv[]) {
  float frequency = mtof(111);

  //  printf("f:%f H:%f\n", frequency, H);
  //  exit(1);

  // find the highest harmonic to use
  float H = 0;
  while (H * frequency < (SAMPLE_RATE / 2)) H++;
  H--;

  float increment = frequency / SAMPLE_RATE;
  float phase = 0;
  for (unsigned _ = 0; _ < 100000; ++_) {
    float sum = 0.0f;

    float amplitude = 0;
    for (int i = 0; i < H; ++i) {
      // https://en.wikipedia.org/wiki/Triangle_wave
      float n = 2 * i + 1;
      sum += powf(-1, i) * powf(n, -2) * sin(n * phase);
      amplitude += powf(n, -2);
    }

    printf("%f\n", 0.707f * sum / amplitude);

    phase += increment;
  }
}
