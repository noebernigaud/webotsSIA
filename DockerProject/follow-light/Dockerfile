FROM osrf/ros:foxy-desktop

WORKDIR /app

COPY . .

RUN gcc -Wall follow-light.c -o follow-light

CMD ["./follow-light"]
#ENTRYPOINT [ "/bin/bash" ]

#Build docker build -t webots .
#RUN docker run --add-host=host.docker.internal:host-gateway --name iwebots  webots