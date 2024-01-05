#include <webots/robot.h>
#include <webots/supervisor.h>
#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "structures.h"

#define TIME_STEP 32

char position_string[100], name_string[100];
char floor_position_string[100];
float normalizer = 0.2;
void spawn_maze(int **matrix, int rows, int columns, WbFieldRef children_field);


int main(int argc, char **argv) {
  
  wb_robot_init();
  
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef children_field = wb_supervisor_node_get_field(root_node, "children");
  
  int width = 3, height = 3, seed = 23;

  FILE *fp;
  fp = fopen("input.txt", "r");
  if (fp == NULL) {
      printf("Error opening the file.\n");
      return 1; 
  }
  fscanf(fp, "%d %d %d", &width, &height, &seed);
  fclose(fp);
  int **maze = generate_maze(&width, &height, seed);
  
  sprintf(floor_position_string, "Basic_floor {translation %f %f 0, size %f %f}", 0.5 * normalizer * (-1 + width), 0.5 * normalizer * (1 - height), normalizer * width, normalizer * height);              
  wb_supervisor_field_import_mf_node_from_string(children_field, 2, floor_position_string);
  
  spawn_maze(maze, width, height, children_field);

  while (wb_robot_step(TIME_STEP) != -1) {
    //tracker?
  };

  wb_robot_cleanup();

  return 0;
}

void spawn_maze(int **matrix, int rows, int columns, WbFieldRef children_field)
{
    
    for (int i = 0; i < columns; i++)
        for (int j = 0; j < rows; j++)
        {
            if (matrix[i][j] == 1)
            {
                sprintf(position_string, "wall_proto {translation %f %f %f}", normalizer * j, normalizer *(-1) * i, normalizer * 0.5);
                wb_supervisor_field_import_mf_node_from_string(children_field, -1, position_string);
            }
        }
}
