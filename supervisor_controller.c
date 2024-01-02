#include <webots/robot.h>
#include <webots/supervisor.h>
#include <stdio.h>

#define TIME_STEP 32

//int** maze(int number_of_rows, int number_of_collumns) functie care genereaza matricea
void spawn_maze(int **matrix){
    
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

int main(int argc, char **argv) {
    wb_robot_init();

    WbNodeRef root_node = wb_supervisor_node_get_root();
    WbFieldRef children_field = wb_supervisor_node_get_field(root_node, "children");

    char position_string[100];
    float normalizer = 0.2; //the size of the wall
    //scanf pt rows si collumns??
    //spawn_maze(maze(rows, collumns);

    while (wb_robot_step(TIME_STEP) != -1) {

        
    }

    wb_robot_cleanup();

    return 0;
}
