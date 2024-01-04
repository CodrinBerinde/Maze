#include <webots/robot.h>
#include <webots/supervisor.h>
#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "structures.h"

#define TIME_STEP 32

char position_string[100];
int width = 10, height = 10, seed = 17;
float normalizer = 0.2;

void spawn_maze(int **matrix, int rows, int collumns)
{

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < collumns; j++)
        {
            if (matrix[i][j] == 1)
            {
                sprintf(position_string, "wall_proto {translation %f %f %f}", normalizer * i, normalizer * j, normalizer * 0.5);
                wb_supervisor_field_import_mf_node_from_string(children_field, -1, position_string);
            }
        }
}

int main(int argc, char **argv)
{
    wb_robot_init();
    
    srand(seed);
    int **maze = generate_maze(&width, &height);

    WbNodeRef root_node = wb_supervisor_node_get_root();
    WbFieldRef children_field = wb_supervisor_node_get_field(root_node, "children");
    
    spawn_maze(maze, 2 * width + 1, 2 * height + 1);

    while (wb_robot_step(TIME_STEP) != -1)
    {
    }

    wb_robot_cleanup();

    return 0;
}
