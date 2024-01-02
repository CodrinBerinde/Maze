#ifndef MAZE_H_INCLUDED
#define MAZE_H_INCLUDED

#include "structures.h"

static int random_dir(int max_dir);

static int in_matrix(int x, int y, int xmax, int ymax);

static int **normalize_maze(int width, int height, int **horizontal_walls, int **vertical_walls);

static int direction_choice(int *xnew, int *ynew, int x, int y, int xmax, int ymax, int **visited);

static void initialize_walls(int width, int height, int ***horizontal_walls, int ***vertical_walls);

static int **initialize_visited(int width, int height);

static void smash_walls(int x, int y, int direction, int **horizontal_walls, int **vertical_walls);

static void dfs(int x, int y, int xmax, int ymax, cell *top, int **horizontal_walls, int **vertical_walls, int **visited);

static void clear_mem(int height, int **matrix);

extern void print_maze(int width, int height, int **maze_matrix);

extern int **generate_maze(int *width, int *height);

#endif