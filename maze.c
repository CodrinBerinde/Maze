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

int **initialize_parent(int width, int height, int xstart, int ystart) {
  int **parent = (int **)calloc(sizeof(int *), height);
  for(int row = 0; row < height; row++) {
    *(parent + row) = (int *)calloc(sizeof(int), width);
    for(int column = 0; column < width; column++) {
      *(*(parent + row) + column) = -1;
    }
  }
  *(*(parent + xstart) + ystart) = -2;
  return parent;
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
  if(is_empty(top)) {
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

int is_valid_direction(int x, int y, int width, int height, int **maze, int **parent) {
  return (in_matrix(x, y, height, width) &&
          (*(*(maze + x) + y) == 0) &&
          (*(*(parent + x) + y) == -1));
}

void bfs(int xstart, int ystart, int width, int height, int **maze, int **parent) {
  cell *front = queue_add(NULL, xstart, ystart);
  cell *back = front;
  
  while(!is_empty(front)) {
    int curr_x = front->x;
    int curr_y = front->y;
  
    //we test all the four directions
    for(int direction = 0; direction < 4; direction++) {
      int new_x = curr_x + relative_dir[direction][0];
      int new_y = curr_y + relative_dir[direction][1];
      if(is_valid_direction(new_x, new_y, width, height, maze, parent)) {
        *(*(parent + new_x) + new_y) = direction;
        back = queue_add(back, new_x, new_y);
      }
    }
    front = rem(front);
  }
}

void clear_mem(int height, int **matrix) {
  for(int row = 0; row < height; row++) {
    free(*(matrix + row));
  }
  free(matrix);
}

int direction_relativization(int current_dir, int prev_dir) {
  if(current_dir == prev_dir) { //forward
    return 0;
  }
  if((current_dir + 4 - prev_dir) % 4 == 1) { //turn left
    return 2;
  }
  //turn left
  return 1;
}

int *reconfigure_path(int xstop, int ystop, int **parent, int *length) {
  cell *top = stack_add(NULL, 0, 0);
  int curr_x = xstop, curr_y = ystop;
  int previous_direction = 0, actual_direction, i = 0;
  *length = 1;
  while(*(*(parent + curr_x) + curr_y) != -2) {
    actual_direction = (*(*(parent + curr_x) + curr_y) + 2) % 4;
    previous_direction = *(*(parent + curr_x) + curr_y);
    curr_x = curr_x + relative_dir[actual_direction][0];
    curr_y = curr_y + relative_dir[actual_direction][1];
    top = stack_add(top, direction_relativization(*(*(parent + curr_x) + curr_y), previous_direction), 0);
    (*length)++;
  }
  
  int *path = (int *)calloc(sizeof(int), *length);
  while(!is_empty(top)) {
    *(path + i) = top->x;
    i++;
    top = rem(top);
  }
  //the first element is 0
  *path = 0;
  //the last element marks the end, and is -1
  *(path + *length - 1) = -1;

  return path;
}

void print_maze(int width, int height, int **maze_matrix) {
  for(int row = 0; row < height; row++) {
    for(int column = 0; column < width; column++) {
      *(*(maze_matrix + row) + column) ? printf("%c", 219) : printf(" ");
    }
    printf("\n");
  }
}

int **generate_maze(int *width, int *height, int seed) {
  int **visited, **horizontal_walls, **vertical_walls;
  int **maze;
  cell *top = NULL;

  srand(seed);

  visited = initialize_visited(*width, *height);
  initialize_walls(*width, *height, &horizontal_walls, &vertical_walls);

  top = stack_add(top, *height - 1, *width - 1);
  *(*(visited + *height - 1) + *width - 1) = 1;
  dfs(*height - 1, *width - 1, *height, *width, top, horizontal_walls, vertical_walls, visited);
  
  maze = normalize_maze(*width, *height, horizontal_walls, vertical_walls);
  clear_mem(*height + 1, horizontal_walls);
  clear_mem(*height, vertical_walls);
  clear_mem(*height, visited);
  *height = 2 * *height + 1;
  *width = 2 * *width + 1;
  return maze;
}

int *solve_maze(int width, int height, int **maze, int xstart, int ystart, int xstop, int ystop) {
  int **parent = initialize_parent(width, height, xstart, ystart);
  int length = 0;
  bfs(xstart, ystart, width, height, maze, parent);

  int *path = reconfigure_path(xstop, ystop, parent, &length);
  clear_mem(height, parent);
  return path;
}