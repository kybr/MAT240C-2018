#include "everything.h"

// XXX this is broken; it does not process arguments... yet

int main(int argc, char* argv[]) {
  float frequency = 220, value = 0;
  float perSampleIncrement = frequency / SAMPLE_RATE;
  while (true) {
    value += perSampleIncrement;
    if (value > 1) value -= 1;
    printf("%f\n", value);
  }
}
