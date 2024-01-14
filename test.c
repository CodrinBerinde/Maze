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
  solve_maze(width, height, maze, 1, 0, height - 2, width - 1, "vv.txt");
  //FILE *fp;
  //fp = fopen("latex.txt", "w");
  //for(int i = 0; i < height; i++) {
  //  for(int j = 0; j < width; j++) {
      //if(maze[i][j])
        //fprintf(fp, "\\filldraw[draw=culoaretext, fill=culoaretext](%d, %d) rectangle (%d, %d);\n", j, -i, j + 1, -(i + 1));
  //  }
  //}
  //fclose(fp);

  //int *path = solve_maze(width, height, maze, 1, 0, height - 2, width - 1);
  //int i = 0;
  //while(*(path + i) != -1) {
  //  printf("%d ", *(path + i));
  //  i++;
  //}
  clear_mem(height, &maze);
  
  return 0;
}