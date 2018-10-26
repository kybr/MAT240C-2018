#include "everything.h"

// accept and pass on only so many inputs

int main(int argc, char* argv[]) {
  unsigned many = strtoul(argv[1], nullptr, 0);
  if (many <= 0) return 0;

  char buffer[256];
  while (many > 0) {
    many--;
    scanf("%s", buffer);
    printf("%s\n", buffer);
  }
}
