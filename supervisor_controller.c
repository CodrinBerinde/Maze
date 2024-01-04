#include <webots/robot.h>
#include <webots/supervisor.h>
#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "structures.h"

#define TIME_STEP 32

char position_string[100], name_string[100];
char floor_position_string[140];
int width = 10, height = 10, seed = 17;
float normalizer = 0.2;

void spawn_maze(int **matrix, int rows, int columns, WbFieldRef children_field);


int main(int argc, char **argv)
{
    wb_robot_init();
    
    WbNodeRef root_node = wb_supervisor_node_get_root();
    WbFieldRef children_field = wb_supervisor_node_get_field(root_node, "children");
    
    srand(seed);
    int **maze = generate_maze(&width, &height);

    sprintf(floor_position_string, "Basic_floor {translation %f %f 0, size %f %f}", 0.5 * normalizer * (-1 + width), 0.5 * normalizer * (-1 + height), normalizer * width, normalizer * height);              
    wb_supervisor_field_import_mf_node_from_string(children_field, 2, floor_position_string);

    spawn_maze(maze, width, height, children_field);

    while (wb_robot_step(TIME_STEP) != -1)
    {
    }

    wb_robot_cleanup();

    return 0;
}

void spawn_maze(int **matrix, int rows, int columns, WbFieldRef children_field)
{
    
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
        {
            if (matrix[i][j] == 1)
            {
                sprintf(position_string, "wall_proto {translation %f %f %f}", normalizer * i, normalizer * j, normalizer * 0.5);
                wb_supervisor_field_import_mf_node_from_string(children_field, -1, position_string);
            }
        }
}
