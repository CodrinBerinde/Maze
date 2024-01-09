/*
  control only the velocity of each motor relying on gps.
 */

#include <webots/distance_sensor.h>
#include <webots/position_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>
#include <webots/gps.h>
#include "maze.h"
#include "structures.h"


#include <stdio.h>

#define TIME_STEP 16
#define PI 3.141592
#define SPEED 3
#define WHEEL 0.04
#define RATIO 1.531

float xcoord[100], ycoord[100];



int main(int argc, char **argv) {
  wb_robot_init();

  WbDeviceTag fr_motor = wb_robot_get_device("front_right_motor");
  WbDeviceTag fl_motor = wb_robot_get_device("front_left_motor");
  WbDeviceTag gps = wb_robot_get_device("gps");

  int i = 0;

  wb_gps_enable(gps, TIME_STEP);
  //int icoord[100] = {1, 1, 2, 3, 4, 5, 6, 7, 7, 8, 9, 9, 9, 9, 9, 9, 9, 9, -1};
  //int jcoord[100] = {0, 1, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  
  
  double left_speed = SPEED;
  double right_speed = SPEED;

  wb_motor_set_velocity(fr_motor, 0);
  wb_motor_set_velocity(fl_motor, 0);
  wb_motor_set_position(fl_motor, INFINITY);
  wb_motor_set_position(fr_motor, INFINITY);

  double *position;
  int t = 0, step = 0;
  float noise = 0.01; 
  int width = 5, height = 5, seed = 23;
  //int **maze = generate_maze(&width, &height, seed);
  printf("test1\n");
  for(int kk = 0; kk < 2000000000; kk++) {

  }
  //print_maze(width, height, maze);
  //int *path = solve_maze(width, height, maze, 1, 0, 9, 10);
  wb_robot_step(TIME_STEP);
  wb_robot_step(TIME_STEP);
  wb_robot_step(TIME_STEP);
  printf("MERE\n");
  int path[100];
  FILE *ff;
  ff = fopen("../supervisor/path.txt", "r");
  //if(fgetc(ff) == EOF) {
  //  printf("Nu e bine\n");
  //}
  //else {
  //  printf("ATENTIE: %c\n", fgetc(ff));
  //}
  int ii = 0;
  int val;
  fscanf(ff, "%d", &val);
  while(val != -1) {
    path[ii++] = val;
    fscanf(ff, "%d", &val);
    printf("%d\n", val);
  }
  path[ii++] = -1;
  printf("test2\n");
  int icoord[100], jcoord[100], coordlen = 1;
  int relative_dir[4][2] = {{0,  1}, {1, 0}, {0, -1}, {-1, 0}};
  int pas = 0, direction = 0;
  int curr_x = 1, curr_y = 0;
  icoord[0] = curr_x;
  jcoord[0] = curr_y;
  while(*(path + pas) != -1) {
    if(*(path + pas) == 1) {
      curr_x = curr_x + relative_dir[direction][0];
      curr_y = curr_y + relative_dir[direction][1];
      direction = (direction + 1) % 4;
    }
    else if(*(path + pas) == 2) {
      curr_x = curr_x + relative_dir[direction][0];
      curr_y = curr_y + relative_dir[direction][1];
      direction = (direction + 4 - 1) % 4;
    }
    
    curr_x = curr_x + relative_dir[direction][0];
    curr_y = curr_y + relative_dir[direction][1];
    //if(*(path + pas + 1) == 0) {
      icoord[coordlen] = curr_x;
      jcoord[coordlen++] = curr_y;
    //}
    pas++;
    
  }
  icoord[coordlen] = height - 2;
  jcoord[coordlen++] = width - 1;

  printf("\nThe coordinates are:\n");
  for(int w = 0; w < coordlen; w++) {
    printf("%d, %d\n", icoord[w], jcoord[w]);
    xcoord[w] = 0.2 * jcoord[w];
    printf("Atribui lui xcoord[%d] = %f\n", w, xcoord[w]);
    ycoord[w] = -0.2 * icoord[w];
    printf("Atribui lui ycoord[%d] = %f\n", w, ycoord[w]);
    
    //printf("desired targets: x %f, y %f\n", xcoord[i], ycoord[i]);
  }

  int gg = 0;
  while(path[gg] != -1) {
    printf("path[%d] = %d\n", gg, path[gg]);
    gg++;
  }
  while (wb_robot_step(TIME_STEP) != -1) {
    position = wb_gps_get_values(gps);
    printf("sunt la x: %f y: %f si vreau sa ajung la x: %f y: %f\n", position[0], position[1], xcoord[step], ycoord[step]);
    //printf("delta x: %f, delta y: %f\n", position[0] - xcoord[step], position[1] - ycoord[step]);

    if (t > 20){
      if (((position[0] >= (xcoord[step] - noise/5)) && (position[0] <= (xcoord[step] + noise))) && ((position[1] >= (ycoord[step] - noise/5)) && (position[1] <= (ycoord[step] + noise))))
      {
        step++;
        printf("step is %d; and path is: %d\n", step, path[step - 1]);
      }
      
      //once the direction has been decided, we can update desired velocities.
      switch (path[step - 1])
      {
      case 0:
        left_speed = SPEED;
        right_speed = SPEED;
        break;
      
      case 1: //turn right
        left_speed = RATIO * SPEED;
        right_speed = SPEED;
        break;
      
      case 2: //turn left
        left_speed = SPEED;
        right_speed = RATIO * SPEED;
        break;
      
      case -1:
        left_speed = 0;
        right_speed = 0;
        break;
      
      default:
        printf("This is forbidden\n");
        break;
      }

      //set desired velocities for each wheel
      wb_motor_set_velocity(fr_motor, right_speed);
      wb_motor_set_velocity(fl_motor, left_speed);
      
    }
    t++;
  };

  
  wb_robot_cleanup();

  return 0;
}
