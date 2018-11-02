#include "Gamma/SoundFile.h"

gam::SoundFile soundFile;
int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Give us a .wav file\n");
    return 1;
  }

  soundFile.path(argv[1]);
  if (!soundFile.openRead()) {
    printf("Could not open %s for reading.\n", argv[1]);
    return 1;
  }

  float* data = new float[soundFile.frames()];
  soundFile.readAll(data);

  for (unsigned i = 0; i < soundFile.frames(); i++) printf("%f\n", data[i]);
}