#include <stdlib.h>
#include <stdio.h>
#include "maze.h"
#include "structures.h"

static int relative_dir[4][2] = {{0,  2}, {2, 0}, {0, -2}, {-2, 0}};

int random_dir(int max_dir) {
  return rand() % max_dir;
}

int in_matrix(int x, int y, int xmax, int ymax) {
  return (x >= 0) && (x < xmax) && (y >= 0) && (y < ymax);
}

int direction_choice(int *x, int *y, int xmax, int ymax, int **visited) {
  //first we establish what directions are valid (not yet visited)
  //and set the bits corresponding to those directions to 1 in the mask, the others are 0
  int option_mask = 0, option_number = 0, choice = 0, option_counter = 0, chosen_direction = 4;
  
  for(int direction = 0; direction < 4; direction++) {
    option_mask *= 2;
    if(in_matrix(*x + relative_dir[direction][0], *y + relative_dir[direction][1], xmax, ymax) &&
       !(*(*(visited + *x + relative_dir[direction][0]) + *y + relative_dir[direction][1]))) {
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
  
  *x = *x + relative_dir[chosen_direction][0];
  *y = *y + relative_dir[chosen_direction][1];

  return chosen_direction;
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

void smash_walls(int x, int y, int direction, int **maze) {
  switch(direction) {
    case 0: //we came from left
      *(*(maze + x) + y - 1) = 0;
      break;
    case 1: //we came from above
      *(*(maze + x - 1) + y) = 0;
      break;
    case 2: //we came from right
      *(*(maze + x) + y + 1) = 0;
      break;
    case 3: //we came from below
      *(*(maze + x + 1) + y) = 0;
      break;
  }
}

void iterative_dfs(int xstart, int ystart, int xmax, int ymax, int **maze, int **visited) {
  cell *top = NULL;
  int curr_x = xstart, curr_y = ystart, old_x = xstart, old_y = ystart, direction;
  
  do {
    direction = direction_choice(&curr_x, &curr_y, xmax, ymax, visited);
    if(direction != -1) {
      *(*(visited + curr_x) + curr_y) = 1;
      top = stack_add(top, old_x, old_y);
      smash_walls(curr_x, curr_y, direction, maze);
    }
    else {
      if(!is_empty(top)) {
        curr_x = top->x;
        curr_y = top->y;
        top = rem(top);
      }
    }
    old_x = curr_x;
    old_y = curr_y;
  } while(!is_empty(top));
}

int call = 0;
float offst = 0.03;

void prtm(int x, int y, int xmax, int ymax, int **maze, int **visited) {
  FILE *fp;
  fp = fopen("latex.txt", "a");
  fprintf(fp, "\\begin{minipage}[b]{0.16\\textwidth}\n"
  "  \\centering\n\\begin{tikzpicture}[scale=0.3]\n");
  for(int row = 0; row < xmax; row++) {
    fprintf(fp, "  \\draw (%d, %.2f) node[left]{\\tiny \\color{culoaretext}%d\\color{black}};\n", 0, -0.5 - row, row);
    for(int col = 0; col < ymax; col++) {
      if(row == 0) {
        fprintf(fp, "  \\draw (%.2f, %d) node[above]{\\tiny \\color{culoaretext}%d\\color{black}};\n", 0.5 + col, 0, col);
      }
      if(maze[row][col] == 1) {
        fprintf(fp, "  \\fill[fill=culoaretext] (%.2f, %.2f) rectangle (%.2f, %.2f);\n", col + offst, -row - offst, col + 1 - offst, -row - 1 + offst);
      }
      else if(visited[row][col] == 1) {
        fprintf(fp, "  \\fill[fill=visitedcell] (%.2f, %.2f) rectangle (%.2f, %.2f);\n", col + offst, -row - offst, col + 1 - offst, -row - 1 + offst);
      }
    }
  }
  fprintf(fp, "  \\fill[fill=currentcell] (%.2f, %.2f) rectangle (%.2f, %.2f);\n", y + offst, -x - offst, y + 1 - offst, -x - 1 + offst);
  for(int dir = 0; dir < 4; dir++) {
    int xn = x + relative_dir[dir][0];
    int yn = y + relative_dir[dir][1];
    if(in_matrix(xn, yn, xmax, ymax) && !visited[xn][yn]) {
      fprintf(fp, "  \\draw[-stealth, directionarrow, thick] (%.2f, %.2f) -- (%.2f, %.2f);\n", 0.5 + y + ((float)(yn - y))/7, -0.5 - x - ((float)(xn - x))/7, 0.5 + yn, -0.5 - xn);
    }
  }
  fprintf(fp, "\\end{tikzpicture}\n"
    "\\caption*{%c)}\n"
  "\\end{minipage}\n"
  "\\hfill\n", (int)('a') + call % 27);
  call++;
  fclose(fp);
}

void recursive_dfs(int x, int y, int xmax, int ymax, int **maze, int **visited) {
  int direction, xante = x, yante = y;
  printf("HELLOO\n");
  prtm(x, y, xmax, ymax, maze, visited);
  int ok = 0;
  
  while((direction = direction_choice(&x, &y, xmax, ymax, visited)) != -1) {
    printf("Sunt la %d, %d si am venit din directia %d\n", x, y, direction);
    *(*(visited + x) + y) = 1;
    smash_walls(x, y, direction, maze);
    recursive_dfs(x, y, xmax, ymax, maze, visited);
    //prtm(x, y, xmax, ymax, maze, visited);
    ok = 1;
  }
  if(ok == 1) {
    prtm(xante, yante, xmax, ymax, maze, visited);
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
      int new_x = curr_x + relative_dir[direction][0] / 2;
      int new_y = curr_y + relative_dir[direction][1] / 2;
      if(is_valid_direction(new_x, new_y, width, height, maze, parent)) {
        *(*(parent + new_x) + new_y) = direction;
        back = queue_add(back, new_x, new_y);
      }
    }
    front = rem(front);
  }
}

void clear_mem(int height, int ***matrix) {
  for(int row = 0; row < height; row++) {
    free(*(*matrix + row));
  }
  free(*matrix);
}

int direction_relativization(int current_dir, int prev_dir) {
  if(current_dir == prev_dir) { //forward
    return 0;
  }
  if((current_dir + 4 - prev_dir) % 4 == 1) { //turn left
    return 2;
  }
  //turn right
  return 1;
}

int *recover_path(int xstop, int ystop, int **parent, char *file_name) {
  cell *top = NULL;
  int curr_x = xstop, curr_y = ystop;
  int next_direction = 0, actual_direction, previous_rel_dir = 0, i = 0;
  int length = 1;
  while(*(*(parent + curr_x) + curr_y) != -2) {
    top = stack_add(top, *(*(parent + curr_x) + curr_y), 0);
    actual_direction = (*(*(parent + curr_x) + curr_y) + 2) % 4;
    curr_x = curr_x + relative_dir[actual_direction][0] / 2;
    curr_y = curr_y + relative_dir[actual_direction][1] / 2;
    length++;
  }
  
  FILE *path_file = fopen(file_name, "w");
  while(!is_empty(top)) {
    actual_direction = top->x;
    if(actual_direction == -2) {
      actual_direction = 0;
    }
    next_direction = (top->link != NULL) ? top->link->x : 0;
    printf("Actual direction is %d and next %d\n", actual_direction, next_direction);
    
    if(previous_rel_dir == 0) {
      fprintf(path_file, "%d %d %d\n", direction_relativization(actual_direction, next_direction), curr_x, curr_y);
    }
    curr_x = curr_x + relative_dir[actual_direction][0] / 2;
    curr_y = curr_y + relative_dir[actual_direction][1] / 2;
    top = rem(top);
    previous_rel_dir = direction_relativization(actual_direction, next_direction);
  }
  fprintf(path_file, "%d %d %d\n", -1, curr_x, curr_y);
  fclose(path_file);
  
  int *path = (int *)calloc(sizeof(int), length);
/*
  while(!is_empty(top)) {
    *(path + i) = top->x;
    i++;
    top = rem(top);
  }
  //the first element is 0
  *path = 0;
  //the last element marks the end, and is -1
  *(path + length - 1) = -1;
*/
  return path;
}

void print_maze(int width, int height, int **maze) {
  for(int row = 0; row < height; row++) {
    for(int column = 0; column < width; column++) {
      *(*(maze + row) + column) ? printf("%c", 219) : printf(" ");
    }
    printf("\n");
  }
}

int **initialize_maze(int width, int height) {
  int **maze = (int **)calloc(sizeof(int *), height);
  for(int row = 0; row < height; row++) {
    
    *(maze + row) = (int *)calloc(sizeof(int), width);
    for(int column = 0; column < width; column++) {
      *(*(maze + row) + column) = ((row % 2 == 1) && (column % 2 == 1)) ? 0 : 1;
    }
  }
  printf("here\n");
  *(*(maze + 1)) = 0;
  *(*(maze + height - 2) + width - 1) = 0;
  return maze;
}

int **generate_maze(int *width, int *height, int seed) {

  (*width) += ((*width) % 2 == 0) ? 1 : 0;
  (*height) += ((*height) % 2 == 0) ? 1 : 0;

  srand(seed);

  int **visited = initialize_visited(*width, *height);

  int **maze = initialize_maze(*width, *height);

  FILE *fp;
  fp = fopen("latex.txt", "w");
  fprintf(fp, "\\begin{figure}[!h!b!t]\n"
  "\\centering\n");
  fclose(fp);
  //*(*(visited + *height - 2) + *width - 2) = 1;
  *(*(visited + 1) + 1) = 1;
  //recursive_dfs(*height - 2, *width - 2, *height, *width, maze, visited);
  recursive_dfs(1, 1, *height, *width, maze, visited);
  //iterative_dfs(*height - 2, *width - 2, *height, *width, maze, visited);
  clear_mem(*height, &visited);
  fp = fopen("latex.txt", "a");
  fprintf(fp, "\\caption{Maze generation using DFS}\n"
  "\\label{f1}\n"
"\\end{figure}\n");
  fclose(fp);
  return maze;
}

int *solve_maze(int width, int height, int **maze, int xstart, int ystart, int xstop, int ystop, char *file_name) {
  int **parent = initialize_parent(width, height, xstart, ystart);
  bfs(xstart, ystart, width, height, maze, parent);
  int *path = recover_path(xstop, ystop, parent, file_name);
  clear_mem(height, &parent);
  return path;
}