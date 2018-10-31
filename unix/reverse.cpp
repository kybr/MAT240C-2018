#include "everything.h"

#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
  std::vector<float> data;
  float f;
  while (!std::cin.eof()) {
    std::cin >> f;
    data.push_back(f);
  }

  while (!data.empty()) {
    printf("%f\n", data.back());
    data.pop_back();
  }
}
