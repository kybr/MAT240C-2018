#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

template <typename T, unsigned N = 1000000>
void run(T& t) {
  float min = 1e9f, max = -1e9f, mean = 0;
  for (int i = 0; i < N; i++) {
    float f = t();
    mean += f;
    if (f < min) min = f;
    if (f > max) max = f;
  }
  mean /= N;
  printf("min:%f  mean:%f  max:%f  N:%d\n", min, mean, max, N);
}

int main() {
  Rect rect;
  rect.frequency(220);
  printf("rect: ");
  run(rect);

  Saw saw;
  saw.frequency(220);
  printf("saw: ");
  run(saw);

  Tri tri;
  tri.frequency(220);
  printf("tri: ");
  run(tri);

  Sine sine;
  sine.frequency(220);
  printf("sine: ");
  run(sine);

  Noise noise;
  noise.frequency(220);
  printf("noise: ");
  run(noise);

  Table table;
  table.frequency(220);
  printf("table: ");
  run(table);
}
