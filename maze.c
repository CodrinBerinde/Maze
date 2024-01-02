#include <stdlib.h>
#include <stdio.h>
#include "maze.h"
#include "structures.h"

static int relative_dir[4][2] = {{0,  1}, {1, 0}, {0, -1}, {-1, 0}};

int random_dir(int max_dir) {
  return rand() % max_dir;
}

int in_matrix(int x, int y, int xmax, int ymax) {
  return (x >= 0) && (x < xmax) && (y >= 0) && (y < ymax);
}

int **normalize_maze(int width, int height, int **horizontal_walls, int **vertical_walls) {
  int normalized_height = 2 * height + 1;
  int normalized_width = 2 * width + 1;
  //allocate the normalized_maze matrix
  int **normalized_maze;
  normalized_maze = (int **)calloc(sizeof(int *), normalized_height);
  for(int row = 0; row < normalized_height; row++) {
    *(normalized_maze + row) = (int *)calloc(sizeof(int), normalized_width);
  }
  
  for(int row = 0; row < height; row++) {
    //the walled row
    for(int column = 0; column < width; column++) {
      *(*(normalized_maze + 2 * row) + 2 * column) = 1;
      *(*(normalized_maze + 2 * row) + 2 * column + 1) = *(*(horizontal_walls + row) + column);
    }
    //we put the last column
    *(*(normalized_maze + 2 * row) + 2 * width) = 1;

    //the hallway row
    for(int column = 0; column < width; column++) {
      *(*(normalized_maze + 2 * row + 1) + 2 * column) = *(*(vertical_walls + row) + column);
      *(*(normalized_maze + 2 * row + 1) + 2 * column + 1) = 0;
    }
    //we put the last column
    *(*(normalized_maze + 2 * row + 1) + 2 * width) = *(*(vertical_walls + row) + width);
  }
  //we put the last row
  for(int column = 0; column < width; column++) {
    *(*(normalized_maze + 2 * height) + 2 * column) = 1;
    *(*(normalized_maze + 2 * height) + 2 * column + 1) = *(*(horizontal_walls + height) + column);
  }
  //we put the last column of the last row
  *(*(normalized_maze + 2 * height) + 2 * width) = 1;

  return normalized_maze;
}

int direction_choice(int *xnew, int *ynew, int x, int y, int xmax, int ymax, int **visited) {
  //first we establish what directions are valid (not yet visited)
  //and set the bits corresponding to those directions to 1 in the mask, the others are 0
  int option_mask = 0, option_number = 0, choice = 0, option_counter = 0, chosen_direction = 4;
  
  for(int direction = 0; direction < 4; direction++) {
    option_mask *= 2;
    if(in_matrix(x + relative_dir[direction][0], y + relative_dir[direction][1], xmax, ymax) &&
       !(*(*(visited + x + relative_dir[direction][0]) + y + relative_dir[direction][1]))) {
      option_number++;
      option_mask += 1;
    }
  }
  
  if(option_number == 0) {
    return -1;
  }

  choice = random_dir(option_number);
  choice++;
  
  while(option_counter != choice) {
    if(option_mask % 2 == 1) {
      option_counter++;
    }
    chosen_direction--;
    option_mask /= 2;
  }
  
  *xnew = x + relative_dir[chosen_direction][0];
  *ynew = y + relative_dir[chosen_direction][1];

  return chosen_direction;
}

void initialize_walls(int width, int height, int ***horizontal_walls, int ***vertical_walls) {
  *horizontal_walls = (int **)calloc(sizeof(int *), height + 1);
  for(int row = 0; row <= height; row++) {
    *(*horizontal_walls + row) = (int *)calloc(sizeof(int), width);
    for(int column = 0; column < width; column++) {
      *(*(*horizontal_walls + row) + column) = 1;
    }
  }

  *vertical_walls = (int **)calloc(sizeof(int *), height);
  for(int row = 0; row < height; row++) {
    *(*vertical_walls + row) = (int *)calloc(sizeof(int), width + 1);
    for(int column = 0; column <= width; column++) {
      *(*(*vertical_walls + row) + column) = 1;
    }
  }

  //we eliminate the entry gate and the finish gate
  *(*(*vertical_walls)) = 0;
  *(*(*vertical_walls + height - 1) + width) = 0;
}

int **initialize_visited(int width, int height) {
  int **visited = (int **)calloc(sizeof(int *), height);
  for(int row = 0; row < height; row++) {
    *(visited + row) = (int *)calloc(sizeof(int), width);
  }
  return visited;
}

void smash_walls(int x, int y, int direction, int **horizontal_walls, int **vertical_walls) {
  switch(direction) {
    case 0:
      *(*(vertical_walls + x) + y) = 0;
      break;
    case 1:
      *(*(horizontal_walls + x) + y) = 0;
      break;
    case 2:
      *(*(vertical_walls + x) + y + 1) = 0;
      break;
    case 3:
      *(*(horizontal_walls + x + 1) + y) = 0;
      break;
  }
}

void dfs(int x, int y, int xmax, int ymax, cell *top, int **horizontal_walls, int **vertical_walls, int **visited) {
  int xnew = x, ynew = y, direction;
  if(top == NULL) {
    return;
  }
  direction = direction_choice(&xnew, &ynew, x, y, xmax, ymax, visited);
  if(direction != -1) {
    *(*(visited + xnew) + ynew) = 1;
    top = stack_add(top, xnew, ynew);
    
    smash_walls(xnew, ynew, direction, horizontal_walls, vertical_walls);
    
    dfs(xnew, ynew, xmax, ymax, top, horizontal_walls, vertical_walls, visited);
  }
  else {
    top = rem(top);
    if(top != NULL) {
      xnew = top->x;
      ynew = top->y;
      dfs(xnew, ynew, xmax, ymax, top, horizontal_walls, vertical_walls, visited);
    }
  }
}

void clear_mem(int height, int **matrix) {
  for(int row = 0; row < height; row++) {
    free(*(matrix + row));
  }
  free(matrix);
}

void print_maze(int width, int height, int **maze_matrix) {
  for(int row = 0; row < height; row++) {
    for(int column = 0; column < width; column++) {
      *(*(maze_matrix + row) + column) ? printf("%c", 219) : printf(" ");
    }
    printf("\n");
  }
}

int **generate_maze(int width, int height) {
  int **visited, **horizontal_walls, **vertical_walls;
  visited = initialize_visited(width, height);
  initialize_walls(width, height, &horizontal_walls, &vertical_walls);
  cell *top = NULL;
  top = stack_add(top, 0, 0);
  *(*(visited)) = 1;
  dfs(0, 0, height, width, top, horizontal_walls, vertical_walls, visited);
  
  int **maze = normalize_maze(width, height, horizontal_walls, vertical_walls);
  clear_mem(height + 1, horizontal_walls);
  clear_mem(height, vertical_walls);
  clear_mem(height, visited);
  print_maze(2 * width + 1, 2 * height + 1, maze);
  return maze;
}