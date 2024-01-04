#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "maze.h"

int main() {
  int width, height, seed;
  printf("Input width and height of maze: ");
  scanf("%d%d", &width, &height);
  printf("Input seed: ");
  scanf("%d", &seed);
  int **maze = generate_maze(&width, &height, seed);
  print_maze(width, height, maze);
  return 0;
}