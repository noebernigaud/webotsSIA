FROM osrf/ros:foxy-desktop

WORKDIR /app

COPY . .

RUN gcc -Wall braitenberg.c -o braitenberg

#ENTRYPOINT [ "/bin/bash" ]

CMD ["./braitenberg"]
#Build docker build -t webots2 .
#RUN docker run --add-host=host.docker.internal:host-gateway --name iwebots2  webots2