/*
  control only the velocity of each motor relying on gps and on compass
 */

//#include <webots/motor.h>
//#include <webots/robot.h>
//#include <webots/gps.h>
//#include <webots/compass.h>
#include "maze.h"
#include "structures.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_STEP 16
#define SPEED 5
#define RATIO 1.531
#define CELL_WIDTH 0.2

const int velocities[3][2] = {{1, 1}, {1, RATIO}, {RATIO, 1}};

int read_path(int **path, double ***absolute_coord) {
  int steps = 0;
  FILE *path_file = fopen("../supervisor/input.txt", "r");
  fscanf(path_file, "%d", steps);

  (*path) = (int *)calloc(sizeof(int), steps);
  (*absolute_coord) = (double **)calloc(sizeof(double *), steps);
  for(int step = 0; step < steps; step++) {
    *((*absolute_coord) + step) = (double *)calloc(sizeof(double), 2);
    fscanf(path_file, "%d%lf%lf", (*path) + step, *((*absolute_coord) + step) + 1, *((*absolute_coord) + step));
    *(*((*absolute_coord) + step)) = CELL_WIDTH * *(*((*absolute_coord) + step));
    *(*((*absolute_coord) + step) + 1) = (- 1.0) * CELL_WIDTH * *(*((*absolute_coord) + step) + 1);
  }
  fclose(path_file);
  return steps;
}

int exit_condition(int req_step, int req_orientation, int req_sign, int step, double **absolute_coord, double *orientation, double *position) {
  if(req_step == 0)
    return (req_sign * (position[req_orientation] - absolute_coord[step + 1][req_orientation]) >= 0.001);
  return (fabs(orientation[req_orientation]) <= 0.9999);
}

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
  int step = 0, correction = 1;

//initial state of the robot wheels
  wb_motor_set_velocity(fr_motor, 0);
  wb_motor_set_velocity(fl_motor, 0);
  wb_motor_set_position(fl_motor, INFINITY);
  wb_motor_set_position(fr_motor, INFINITY);

//get the path for the robot 
  wb_robot_step(TIME_STEP);
  int *path;
  double **absolute_coord;
  int total_steps = read_path(&path, &absolute_coord);
  int req_step, req_orientation, req_sign;

  while(path[step] != -1) {
    do {
      position = wb_gps_get_values(gps);
      orientation = wb_compass_get_values(compass);
      req_step = path[step];
      req_orientation = cos(orientation[0] < 0.6);
      req_sign = (position[req_orientation] - absolute_coord[step + 1][req_orientation] < 0) ? -1 : 1;

      wb_motor_set_velocity(fr_motor, SPEED * velocities[req_step][0]);
      wb_motor_set_velocity(fl_motor, SPEED * velocities[req_step][1]);
      wb_robot_step(TIME_STEP);
    } while(exit_condition(req_step, req_orientation, req_sign, step, absolute_coord, orientation, position));
    step++;
  }

  wb_motor_set_velocity(fr_motor, 0);
  wb_motor_set_velocity(fl_motor, 0); 
  
  wb_robot_cleanup();

  return 0;
}