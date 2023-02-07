FROM osrf/ros:foxy-desktop

WORKDIR /app

COPY . .

RUN gcc -Wall client.c -o client

ENTRYPOINT [ "/bin/bash" ]

#Build docker build -t webots2 .
#RUN docker run -it --add-host=host.docker.internal:host-gateway --rm --name iwebots2  webots2