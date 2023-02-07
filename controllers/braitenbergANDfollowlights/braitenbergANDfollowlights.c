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
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <webots/led.h>
#include <webots/position_sensor.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>  /* definition of inet_ntoa */
#include <netdb.h>      /* definition of gethostbyname */
#include <netinet/in.h> /* definition of struct sockaddr_in */
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h> /* definition of close */
#endif

#define SOCKET_PORT 10020
#define NB_IR_SENSOR 8
#define TIMESTEP 250

static int fd;
static fd_set rfds;

static int accept_client(int server_fd) {
  int cfd;
  struct sockaddr_in client;
#ifndef _WIN32
  socklen_t asize;
#else
  int asize;
#endif
  struct hostent *client_info ;

  asize = sizeof(struct sockaddr_in);

  cfd = accept(server_fd, (struct sockaddr *)&client, &asize);
  if (cfd == -1) {
    printf("cannot accept client\n");
    return -1;
  }
  client_info = gethostbyname((char *)inet_ntoa(client.sin_addr));
  printf("Accepted connection from: %s \n", client_info->h_name);

  return cfd;
}

static int create_socket_server(int port) {
  int sfd, rc;
  struct sockaddr_in address;

#ifdef _WIN32
  /* initialize the socket api */
  WSADATA info;

  rc = WSAStartup(MAKEWORD(1, 1), &info); /* Winsock 1.1 */
  if (rc != 0) {
    printf("cannot initialize Winsock\n");
    return -1;
  }
#endif
  /* create the socket */
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    printf("cannot create socket\n");
    return -1;
  }

  /* fill in socket address */
  memset(&address, 0, sizeof(struct sockaddr_in));
  address.sin_family = AF_INET;
  address.sin_port = htons((unsigned short)port);
  address.sin_addr.s_addr = INADDR_ANY;

  /* bind to port */
  rc = bind(sfd, (struct sockaddr *)&address, sizeof(struct sockaddr));
  if (rc == -1) {
    printf("cannot bind port %d\n", port);
#ifdef _WIN32
    closesocket(sfd);
#else
    close(sfd);
#endif
    return -1;
  }

  /* listen for connections */
  if (listen(sfd, 1) == -1) {
    printf("cannot listen for connections\n");
#ifdef _WIN32
    closesocket(sfd);
#else
    close(sfd);
#endif
    return -1;
  }
  printf("Waiting for a connection on port %d...\n", port);

  return accept_client(sfd);
}




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
  
  fd = create_socket_server(SOCKET_PORT);
  
  
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
  
  //int counter = 0;

  //RUNNING
  while (wb_robot_step(time_step) != -1) {
    int number;
    int n;
    int ret;
    struct timeval tv = {0, 0};
    char buffer[256];
    //double left_speed, right_speed;
    double speed_forward,speed_diff;
    const double ls0_value = wb_light_sensor_get_value(ls0);
    const double ls1_value = wb_light_sensor_get_value(ls1);
    const double ls2_value = wb_light_sensor_get_value(ls2);
    const double ls3_value = wb_light_sensor_get_value(ls3);
    const double ls4_value = wb_light_sensor_get_value(ls4);
    const double ls5_value = wb_light_sensor_get_value(ls5);
    const double ls6_value = wb_light_sensor_get_value(ls6);
    const double ls7_value = wb_light_sensor_get_value(ls7);
    
  
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
  
    number = select(fd + 1, &rfds, NULL, NULL, &tv);
  
    if (number != 0){
      n = recv(fd, buffer, 256, 0);
      if (n < 0) {
        printf("error reading from socket\n");
      }else{
        buffer[n] = '\0';
        printf("Received %d bytes: %s\n", n, buffer);
        if (buffer[0] == 'L') {
          sprintf(buffer, "L,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\r\n",
            ls0_value,ls1_value,ls2_value,ls3_value,ls4_value,ls5_value,ls6_value,ls7_value);
          printf("L,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",ls0_value,ls1_value,ls2_value,ls3_value,ls4_value,ls5_value,ls6_value,ls7_value) ;           
          send(fd, buffer, strlen(buffer), 0);
        }else if(buffer[0] == 'R'){
          sscanf(buffer, "R,%lf,%lf\r\n", &speed_forward, &speed_diff);
          sprintf(buffer, "L,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\r\n",
            ls0_value,ls1_value,ls2_value,ls3_value,ls4_value,ls5_value,ls6_value,ls7_value);
          //printf("L,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",ls0_value,ls1_value,ls2_value,ls3_value,ls4_value,ls5_value,ls6_value,ls7_value);      
          printf("R,%lf,%lf \n",speed_forward, speed_diff);
          send(fd, buffer, strlen(buffer), 0); 
        }else if (strncmp(buffer, "exit", 4) == 0) {
          printf("connection closed\n");
          #ifdef _WIN32
            closesocket(fd);
            ret = WSACleanup();
          #else
            ret = close(fd);
          #endif
          if (ret != 0) {
            printf("Cannot close socket\n");
          }
          fd = 0;
          return(0);
        }else{
          send(fd, "\n", 1, 0);
        }
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
        
        /*
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
        
        if(speed_forward < 0.0){
          speed_forward = 0.0;
          if(abs(speed_diff) < 1){
           speed_diff = 1;
          }
        }
        printf("R,%lf,%lf \n",speed_forward, speed_diff);
        */
        /* Set the motor speeds. */
        
        wb_motor_set_velocity(left_motor, speed_forward + speed_diff + (braintenberg_speed[0] - 3) * 3);
        wb_motor_set_velocity(right_motor, speed_forward - speed_diff + (braintenberg_speed[1] - 3) * 3);
      }
    }
  }

  return 0;
}

