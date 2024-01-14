#include <stdlib.h>
#include <stdio.h>

int main() {
  int *p;
  p = (int *)calloc(sizeof(int), 7);
  p[3] = 19;
  free(p);
  printf("here: %d", p[3]);
  return 0;
}