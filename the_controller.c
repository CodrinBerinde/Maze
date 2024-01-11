/*
  control only the velocity of each motor relying on gps and on compass
 */

#include <webots/motor.h>
#include <webots/robot.h>
#include <webots/gps.h>
#include <webots/compass.h>
#include "maze.h"
#include "structures.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 16
#define PI 3.141592
#define SPEED 5
#define WHEEL 0.04
#define RATIO 1.531

float xcoord[100], ycoord[100];

int main(int argc, char **argv) {
  wb_robot_init();

  WbDeviceTag fr_motor = wb_robot_get_device("front_right_motor");
  WbDeviceTag fl_motor = wb_robot_get_device("front_left_motor");
  WbDeviceTag gps = wb_robot_get_device("gps");
  WbDeviceTag compass = wb_robot_get_device("compass");

  wb_gps_enable(gps, TIME_STEP);
  wb_compass_enable(compass, TIME_STEP);  
//the variables that wee need to use:  
  double left_speed = SPEED, right_speed = SPEED;
  double *position, *orientation;
  int step = 0;
  

  //int width = 5, height = 5, seed = 23;

  wb_motor_set_velocity(fr_motor, 0);
  wb_motor_set_velocity(fl_motor, 0);
  wb_motor_set_position(fl_motor, INFINITY);
  wb_motor_set_position(fr_motor, INFINITY);

//get the path for the robot 
  wb_robot_step(TIME_STEP);
  int path[100];
  FILE *ff;
  ff = fopen("../supervisor/path.txt", "r");
  int ii = 0;
  int val;
  fscanf(ff, "%d", &val);
  while(val != -1) {
    path[ii++] = val;
    fscanf(ff, "%d", &val);
    printf("%d\n", val);
  }
  path[ii++] = -1;

//get width and height
  int width, height;
  FILE *fp;
  fp = fopen("../supervisor/input.txt", "r");
  fscanf(fp, "%d %d", &width, &height); //hope this is correct
  fclose(fp);

//for path get the abosolute coordinates in i and j; might not need this many.   
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
    icoord[coordlen] = curr_x;
    jcoord[coordlen++] = curr_y;
    pas++; 
  }
  icoord[coordlen] = height - 2;
  jcoord[coordlen++] = width - 1;
//checking the coordinates  
  printf("\nThe coordinates are:\n");
  for(int w = 0; w < coordlen; w++) {
    printf("%d, %d\n", icoord[w], jcoord[w]);
    xcoord[w] = 0.2 * jcoord[w];
    printf("Atribui lui xcoord[%d] = %f\n", w, xcoord[w]);
    ycoord[w] = -0.2 * icoord[w];
    printf("Atribui lui ycoord[%d] = %f\n", w, ycoord[w]);
  }

//checking the path
  int gg = 0;
  while(path[gg] != -1) {
    printf("path[%d] = %d\n", gg, path[gg]);
    gg++;
  }

//the main loop
  while (wb_robot_step(TIME_STEP) != -1) {
    //the things that wee need at each time step (might need to be moved inside a while loop below)
    position = wb_gps_get_values(gps);   //position[0] stores the x value, position[1] stroes the y value
    orientation = wb_compass_get_values(compass); //orientation[0] stores the direction relative to positive Y axis
//for each value of path we move the robot:
    if ((path[step] == 0) && (cos(orientation[0]) > 0.9)) //moves along x axis
    {
      int correction = 1;
      if(position[0] - xcoord[step] < 0)
        correction = 1;
      else correction = -1;
      printf("before while\n");
      while (correction * (position[0] - xcoord[step]) <= -0.001) //the robot hasn't touched the line defined by xcoord[step]
      {
        wb_motor_set_velocity(fr_motor, SPEED);
        wb_motor_set_velocity(fl_motor, SPEED); 
        wb_robot_step(TIME_STEP);  //send this data to the simulation (and receive the new data for computing); the simulation runs for TIME_STEP ms
        position = wb_gps_get_values(gps);
        printf("the position of the robot obtained inside our while loop is %lf\n", position[0]);
      }
      printf("after while\n");
      step++;     //acest if este responsabil de deplasarea robotului inainte de-a lungul axei x. Mai trebuie facut asemanator pentru axa y si pentru luat curba
                  //trebuie verificat daca merge cand robotul merge spre x negativ. Cred ca doar cateva ifuri din astea mai trebuie si gata :)
    }
    

  };

  
  wb_robot_cleanup();

  return 0;
}
