#include <cstdio>

int main(int argc, char** argv) {
  for (int i = 0; i < argc; i++) {
    printf("%d: %s\n", i, argv[i]);
  }

  char buffer[1024];
  buffer[0] = '\0';
  gets_s(&buffer[0], 1024);
  printf("%s\n", buffer);
  return argc;
}