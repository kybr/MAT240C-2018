#include "everything.h"

// XXX this is broken; it does not process arguments... yet

int main(int argc, char* argv[]) {
  float frequency = 220, amplitude = 0.707, phase = 0;

  // argv[0] the name of the program: "sine.exe"
  // argv[1] == "231.4" if someone said something like "./sine.exe 231.4"

  if (argc == 2) frequency = atof(argv[1]);

  float perSampleIncrement = frequency / SAMPLE_RATE * 2 * M_PI;
  while (true) {
    phase += perSampleIncrement;
    printf("%f\n", sin(phase) * amplitude);
  }
}
