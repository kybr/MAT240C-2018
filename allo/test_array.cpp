#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

int main() {
  Array ar;
  ar.resize(100);
  for (int i = 0; i < ar.size; i++) ar.data[i] = (float)i / ar.size;
  int i = 0;
  for (float f = -222.222; f < 222.222; f += 10.1111)
    printf("%d: (%f, %f)\n", i++, f, ar.get(f));
}
