/*
 * Description:  A controller moving Khepera II robot robots 
 * with a combination of obstacle avoidance with braitenberg and light following
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <webots/distance_sensor.h>
#include <webots/light_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define MAX_SENSOR_NUMBER 16
#define RANGE (1024 / 2)
#define BOUND(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

#define MAX_SPEED 10
#define SPEED 6
#define TIME_STEP 64

static WbDeviceTag sensors[MAX_SENSOR_NUMBER], left_motor, right_motor;
static double matrix[MAX_SENSOR_NUMBER][2];
static int num_sensors;
static double range;
static int time_step = 0;
static double max_speed = 0.0;
static double speed_unit = 1.0;

int main() {
  //INITIALIZATION
  wb_robot_init();
  
  //BRAINTENBERG SETTINGS
  time_step = wb_robot_get_basic_time_step();
  const char khepera_name[] = "ds0";
  char sensors_name[5];
  const double(*temp_matrix)[2];
  range = RANGE;
  const double khepera_matrix[8][2] = {{-2, 4}, {-3, 5}, {-7, 7}, {7, -6}, {5, -4}, {4, -2}, {-0.5, -0.5}, {-0.5, -0.5}};
  const double khepera_max_speed = 10.0;
  const double khepera_speed_unit = 1.0;

  num_sensors = 8;
  sprintf(sensors_name, "%s", khepera_name);
  temp_matrix = khepera_matrix;
  max_speed = khepera_max_speed;
  speed_unit = khepera_speed_unit;

  int i;
  for (i = 0; i < num_sensors; i++) {
    sensors[i] = wb_robot_get_device(sensors_name);
    wb_distance_sensor_enable(sensors[i], time_step);

    if ((i + 1) >= 10) {
      sensors_name[2] = '1';
      sensors_name[3]++;

      if ((i + 1) == 10) {
        sensors_name[3] = '0';
        sensors_name[4] = '\0';
      }
    } else
      sensors_name[2]++;

    int j;
    for (j = 0; j < 2; j++)
      matrix[i][j] = temp_matrix[i][j];
  }

  //FOLLOWLIGHT SETTINGS
  WbDeviceTag ls0, ls1, ls2, ls3, ls4, ls5, ls6, ls7;
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

  printf("Move the light (shift + drag mouse), the robot should follow it while avoiding obstacles.\n");
  
  int counter = 0;

  //RUNNING
  while (wb_robot_step(time_step) != -1) {

    //BRAINTENBERG
    int i, j;
    double braintenberg_speed[2];
    double sensors_value[MAX_SENSOR_NUMBER];

    for (i = 0; i < num_sensors; i++)
      sensors_value[i] = wb_distance_sensor_get_value(sensors[i]);

    for (i = 0; i < 2; i++) {
      braintenberg_speed[i] = 0.0;
      for (j = 0; j < num_sensors; j++) {
        braintenberg_speed[i] += speed_unit * matrix[j][i] * (1.0 - (sensors_value[j] / range));
      }
      braintenberg_speed[i] = BOUND(braintenberg_speed[i], -max_speed, max_speed);
    }
    //SI BRAINTEBERG ORDONNE DE TOURNER ON SUIT SES INSTRUCTIONS CAR CELUI CI EST PLUS CRITIQUE
    /*if(abs(braintenberg_speed[0]-braintenberg_speed[1]) > 0.0 || counter > 0){
      wb_motor_set_velocity(left_motor, braintenberg_speed[0]);
      wb_motor_set_velocity(right_motor, braintenberg_speed[1]);
      counter += 1;
      if(counter > 10) counter = 0;
    } else {*/
    //SI BRAINTENBERG NE DIT PAS DE TOURNER ON APPLIQUE LE LIGHT FOLLOWING
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
      
      double left_speed = (left_total_value - 20) / 60.0;
      left_speed = (left_speed < MAX_SPEED) ? left_speed : MAX_SPEED;
      double right_speed = (right_total_value -20) / 60.0;
      right_speed = (right_speed < MAX_SPEED) ? right_speed : MAX_SPEED;
      
      double speed_diff = left_speed - right_speed;
      
      double speed_forward = (behind - ahead) / 60.0;
      speed_forward = (speed_forward < (MAX_SPEED - 1 - abs(speed_diff))) ? speed_forward : (MAX_SPEED - 1 - abs(speed_diff));
  
      /* Set the motor speeds. */
      wb_motor_set_velocity(left_motor, speed_forward + speed_diff + (braintenberg_speed[0] - 3) * 3);
      wb_motor_set_velocity(right_motor, speed_forward - speed_diff + (braintenberg_speed[1] - 3) * 3);
    //}
  }

  return 0;
}

