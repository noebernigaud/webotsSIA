/*
 * Copyright 1996-2022 Cyberbotics Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Description:  A controller moving various robots using the Braitenberg method.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <webots/distance_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define MAX_SENSOR_NUMBER 16
#define RANGE (1024 / 2)
#define BOUND(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

static WbDeviceTag sensors[MAX_SENSOR_NUMBER], left_motor, right_motor;
static double matrix[MAX_SENSOR_NUMBER][2];
static int num_sensors;
static double range;
static int time_step = 0;
static double max_speed = 0.0;
static double speed_unit = 1.0;

static void initialize() {
  /* necessary to initialize Webots */
  wb_robot_init();
  time_step = wb_robot_get_basic_time_step();
  const char *robot_name = wb_robot_get_name();
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

  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

  printf("The %s robot is initialized, it uses %d distance sensors\n", robot_name, num_sensors);
}

int main() {
  initialize();

  while (wb_robot_step(time_step) != -1) {
    int i, j;
    double speed[2];
    double sensors_value[MAX_SENSOR_NUMBER];

    for (i = 0; i < num_sensors; i++)
      sensors_value[i] = wb_distance_sensor_get_value(sensors[i]);

    /*
     * The Braitenberg algorithm is really simple, it simply computes the
     * speed of each wheel by summing the value of each sensor multiplied by
     * its corresponding weight. That is why each sensor must have a weight
     * for each wheel.
     */
    for (i = 0; i < 2; i++) {
      speed[i] = 0.0;
      for (j = 0; j < num_sensors; j++) {
        speed[i] += speed_unit * matrix[j][i] * (1.0 - (sensors_value[j] / range));
      }
      speed[i] = BOUND(speed[i], -max_speed, max_speed);
    }

    /* Set the motor speeds */
    wb_motor_set_velocity(left_motor, speed[0]);
    wb_motor_set_velocity(right_motor, speed[1]);
  }

  return 0;
}
