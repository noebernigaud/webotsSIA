#include <stdio.h>
#include <webots/light_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define MAX_SPEED 10
#define SPEED 6
#define TIME_STEP 64

int main() {
  WbDeviceTag ls0, ls1, ls2, ls3, ls4, ls5, ls6, ls7, left_motor, right_motor;

  wb_robot_init();

  /* get a handler to the distance sensors. */
  ls0 = wb_robot_get_device("ls0");
  ls1 = wb_robot_get_device("ls1");
  ls2 = wb_robot_get_device("ls2");
  ls3 = wb_robot_get_device("ls3");
  ls4 = wb_robot_get_device("ls4");
  ls5 = wb_robot_get_device("ls5");
  ls6 = wb_robot_get_device("ls6");
  ls7 = wb_robot_get_device("ls7");

  wb_light_sensor_enable(ls0, TIME_STEP);
  wb_light_sensor_enable(ls1, TIME_STEP);
  wb_light_sensor_enable(ls2, TIME_STEP);
  wb_light_sensor_enable(ls3, TIME_STEP);
  wb_light_sensor_enable(ls4, TIME_STEP);
  wb_light_sensor_enable(ls5, TIME_STEP);
  wb_light_sensor_enable(ls6, TIME_STEP);
  wb_light_sensor_enable(ls7, TIME_STEP);

  /* get a handler to the motors and set target position to infinity (speed control). */
  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

  printf("Move the light (shift + drag mouse), the robot should follow it.\n");
  
  int counter = 0;

  while (wb_robot_step(TIME_STEP) != 1) {
    counter = counter + 1;
    /* read sensor values */
    const double ls0_value = wb_light_sensor_get_value(ls0);
    const double ls1_value = wb_light_sensor_get_value(ls1);
    const double ls2_value = wb_light_sensor_get_value(ls2);
    const double ls3_value = wb_light_sensor_get_value(ls3);
    const double ls4_value = wb_light_sensor_get_value(ls4);
    const double ls5_value = wb_light_sensor_get_value(ls5);
    const double ls6_value = wb_light_sensor_get_value(ls6);
    const double ls7_value = wb_light_sensor_get_value(ls7);

    double right_total_value = (ls3_value + ls4_value + ls5_value + ls6_value) / 4;
    double left_total_value = (ls0_value + ls1_value + ls2_value + ls7_value) / 4;
    
    double behind = (ls6_value + ls7_value) / 2;
    double ahead = (ls2_value + ls3_value) / 2;
    
    if(counter > 50) {
      printf("left: %.0f, right: %.0f\n", left_total_value, right_total_value);
      printf("ahead: %.0f, behind: %.0f\n", ahead, behind);
      counter = 0;
    }
    
    //double left_speed = (512 - left_total_value) / 100.0;
    double left_speed = (left_total_value - 20) / 60.0;
    left_speed = (left_speed < MAX_SPEED) ? left_speed : MAX_SPEED;
    //double right_speed = (512 - right_total_value) / 100.0;
    double right_speed = (right_total_value -20) / 60.0;
    right_speed = (right_speed < MAX_SPEED) ? right_speed : MAX_SPEED;
    
    double speed_diff = left_speed - right_speed;
    
    double speed_forward = (behind - ahead) / 60.0;
    speed_forward = (speed_forward < (MAX_SPEED - 1 - abs(speed_diff))) ? speed_forward : (MAX_SPEED - 1 - abs(speed_diff));

    /* Set the motor speeds. */
    wb_motor_set_velocity(left_motor, speed_forward + speed_diff);
    wb_motor_set_velocity(right_motor, speed_forward - speed_diff);
  }

  wb_robot_cleanup();

  return 0;
}
