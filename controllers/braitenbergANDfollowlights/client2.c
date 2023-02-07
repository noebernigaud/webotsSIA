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
 * Description:  A simple client program to connect to the TCP/IP server thanks to Darren Smith
 */

/*
 * Linux:   compile with gcc -Wall client.c -o client
 * Windows: compile with gcc -Wall -mno-cygwin client.c -o client -lws2_32
 */

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define SOCKET_PORT 10021
#define SOCKET_SERVER "127.0.0.1" /* local host or host.docker.internal */
#define MAX_SPEED 10

#define MAX_SENSOR_NUMBER 16
#define RANGE (1024 / 2)
#define BOUND(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

static double matrix[8][2] = {{-2, 4}, {-3, 5}, {-7, 7}, {7, -6}, {5, -4}, {4, -2}, {-0.5, -0.5}, {-0.5, -0.5}};
static int num_sensors = 8;
static double range = RANGE;
static int time_step = 0;
static double max_speed = 10.0;
static double speed_unit = 1.0;

int main(int argc, char *argv[]) {
  struct sockaddr_in address;
  struct hostent *server;
  int fd, rc;
  char buffer[256];

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
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    printf("cannot create socket\n");
    return -1;
  }

  /* fill in the socket address */
  memset(&address, 0, sizeof(struct sockaddr_in));
  address.sin_family = AF_INET;
  address.sin_port = htons(SOCKET_PORT);
  server = gethostbyname(SOCKET_SERVER);

  if (server)
    memcpy((char *)&address.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
  else {
    printf("cannot resolve server name: %s\n", SOCKET_SERVER);
#ifdef _WIN32
    closesocket(fd);
#else
    close(fd);
#endif
    return -1;
  }

  /* connect to the server */
  rc = connect(fd, (struct sockaddr *)&address, sizeof(struct sockaddr));
  if (rc == -1) {
    printf("cannot connect to the server\n");
#ifdef _WIN32
    closesocket(fd);
#else
    close(fd);
#endif
    return -1;
  }

  for (;;) {
    int n = strlen(buffer);
    sprintf(buffer, "L\r\n");
    n = send(fd, buffer, strlen(buffer), 0);
    n = recv(fd, buffer, 256, 0);
    if(buffer[0]=='L'){
      double sensors_value[MAX_SENSOR_NUMBER];
      sscanf(buffer, "L,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\r\n",
       &sensors_value[0], &sensors_value[1],&sensors_value[2],&sensors_value[3],
       &sensors_value[4],&sensors_value[5],&sensors_value[6],&sensors_value[7]);
      
      int i, j;
      double braintenberg_speed[2];
     
      for (i = 0; i < 2; i++) {
        braintenberg_speed[i] = 0.0;
        for (j = 0; j < num_sensors; j++) {
          braintenberg_speed[i] += speed_unit * matrix[j][i] * (1.0 - (sensors_value[j] / range));
        }
        braintenberg_speed[i] = BOUND(braintenberg_speed[i], -max_speed, max_speed);
      }
      
      sprintf(buffer, "R,%lf,%lf \r\n",braintenberg_speed[0] , braintenberg_speed[1]);
      n=send(fd,buffer,strlen(buffer),0);
    }
  }

#ifdef _WIN32
  closesocket(fd);
#else
  close(fd);
#endif

  return 0;
}
