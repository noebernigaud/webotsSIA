FROM osrf/ros:foxy-desktop

WORKDIR /app

COPY . .

RUN gcc -Wall client.c -o client

ENTRYPOINT [ "/bin/bash" ]