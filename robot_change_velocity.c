/*
  control only the velocity of each motor relying on clock time.
 */

#include <webots/distance_sensor.h>
#include <webots/position_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#include <stdio.h>

#define TIME_STEP 16
#define PI 3.141592
#define SPEED 5
#define WHEEL 0.04


int main(int argc, char **argv) {
  wb_robot_init();

  WbDeviceTag fr_motor = wb_robot_get_device("front_right_motor");
  WbDeviceTag fl_motor = wb_robot_get_device("front_left_motor");
  WbDeviceTag psl = wb_robot_get_device("left_psensor");
  wb_position_sensor_enable(psl, TIME_STEP);
  
  double left_speed = 0;
  double right_speed = 0;

  wb_motor_set_velocity(fr_motor, right_speed);
  wb_motor_set_velocity(fl_motor, left_speed);
  wb_motor_set_position(fr_motor, INFINITY);
  wb_motor_set_position(fl_motor, INFINITY);

  
  double target_pos = 0, position = 0;
  double r = 0.158, R = 0.242, ratio = R / r;


  while (wb_robot_step(TIME_STEP) != -1) {
    position = wb_position_sensor_get_value(psl);

    if (position < 2 * PI * R / WHEEL) // left wheel hasn't walked the fulll big circle
    {
      left_speed = ratio * SPEED; //the robot turns right
      right_speed = SPEED;
    } 
    else
    {
      left_speed = 0;
      right_speed = 0;
    }
    
    wb_motor_set_velocity(fr_motor, right_speed);
    wb_motor_set_velocity(fl_motor, left_speed);
  };

  
  wb_robot_cleanup();

  return 0;
}
