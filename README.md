# Rendue projet

## Exectution du projet:

### Avec Docker:

1) Ouvrir world: khepera2.wbt du projet dans Webots

2) Depuis le terminal aller dans DockerProject puis executer:
   - docker-compose build
   - docker-compose up -d

### En local:

1) Ouvrir world: khepera2.wbt du projet dans Webots

2) lors d'un build du controller braitenbergANDfollowlights il est generer directement client.exe et client2.exe que vous retrouvez dans le dosier /controller/braitenbergANDfollowlights/
d'ici ouvrir deux terminal
- terminal1 ./client
- terminal2 ./client2

## Communication:

Pour cette partie nous nous sommes inspirer du projet khepera-tcpip

### Parti docker:

Les contenaire docker utilise un reseau docker0 avec l'adresse host.docker.internal(adress de docker) qui est connecter au host.
Ainsi ils peuvent ce connecter en tcp/ip vers host.docker.internal:10020 et host.docker.internal:10021.
Il request les donner lumiere et de distance du sensor en envoyant L et renvois avec R %fl %fl les double pour metriser le mouvement du robot.
Pour envoyer et receptioner les donner nous utilisant send(fd, buffer, strlen(buffer), 0); et recv(fd, buffer, 256, 0); 
sscanf permet de distribuer les données depuis le buffer dans les variables.
sprintf permet de distribuer  les données depuis les variables dans le buffer.

### Parti webots

Webots attend connections TCP en localhost sur 10020 puis 10021.
Des qu'ils sont connecter le program attend l'envois des données. 
Lors les deux ont envoyé des commands.
Il recupere c'est donner les assemble et bouge le robot puis renvois les données des sensors.



