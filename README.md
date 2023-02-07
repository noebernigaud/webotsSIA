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

------Noé

# RENDU PROJET SIA

## Vidéo de Démonstration

//video

## Objectif
Ce projet vise à programmer un controlleur ordonnant au robot d'avancer en évitant les obstacles et un autre lui ordonnant de se déplacer en direction de la lumière. Il convient ensuite de combiner ses deux comportements, d'abord directement dans un unique controlleur Webots, puis en isolant les programmes sous forme de micro-services en les assignant chacun à un docker.

## Execution du projet

- Ouvrir world: khepera2.wbt du projet dans Webots
- Depuis le terminal aller dans /DockerProject puis executer:
   - docker-compose build
   - docker-compose up -d

## Travail effectué
- Nous avons instancié un monde Webots et mis en place le robot, une lumière facilement déplaçable ainsi que des obstacles pour la démonstration.
- Le modèle Braitenberg a été utilisé en ajustant les paramètres de celui-ci afin d'appliquer l'algorythme d'évitement d'obstacles.
- Le controlleur permettant de suivre la lumière fonctionne selon la direction où la lumère et la plus forte - si celle ci est plus forte en avant qu'en arrière, il avance, et vice-versa, et la même loique est appliquée pour les directions droite/gauche.
- La combinaison des deux algorythmes est faite en laissant au controlleur en charge du suivi de la lumière la charge d'avancer/reculer et influe aussi sur la direction. L'évitement d'obstacle est quant à lui concentré sur la direction,domaine où il a plus de poid que le controlleur de suivi de lumière puisque l'esquive d'obstacle est de nature plus urgente et critique.
- Nous avons dockerisé les deux controlleurs, qui communiquent alors par des sockets avec le controlleur webots qui lui est en charge de fusionner les entrées des 2 controlleurs. Les deux controlleurs dockerisés sont des microservice - ils sont totalement indépendant et peuvent être deplacé vers d'autres application ou interchangés.
- Un fichier docker-compose permet le lancement facile des différent dockers avec leurs paramètres.




