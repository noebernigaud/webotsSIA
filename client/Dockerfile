FROM osrf/ros:foxy-desktop

WORKDIR /app

COPY . .

RUN gcc -Wall client.c -o client

ENTRYPOINT [ "/bin/bash" ]

#Build docker build -t webots .
#RUN docker run -it --add-host=host.docker.internal:host-gateway --rm --name iwebots3  webots