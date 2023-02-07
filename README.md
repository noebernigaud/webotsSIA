# Rendu projet

## Vidéo de Démonstration

//video

## Objectif
Ce projet vise à programmer un controller ordonnant au robot d'avancer en évitant les obstacles et un autre lui ordonnant de se déplacer en direction de la lumière. Il convient ensuite de combiner ces deux comportements, d'abord directement dans un unique controller Webots, puis en isolant les programmes sous forme de micro-services en les assignant chacun à un docker.

## Execution du projet:

### Avec Docker:

1) Ouvrir world: khepera2.wbt de ce projet dans Webots

2) Depuis le terminal aller dans DockerProject puis exécuter:
   - docker-compose build
   - docker-compose up -d

### Sans Docker:

1) Ouvrir world: khepera2.wbt du projet dans Webots

2) Lors d'un build du controller braitenbergANDfollowlights il est généré directement client.exe et client2.exe que vous retrouvez dans le dossier 
/controller/braitenbergANDfollowlights/ .
D'ici ouvrir deux terminaux
- terminal1 ./client
- terminal2 ./client2


## Travail effectué
- Nous avons mis en place le robot khepera2 de webots.
- Nous avons instancié un monde avec une lumière facilement déplaçable ainsi que des obstacles pour la démonstration.
- Le modèle Braitenberg a été utilisé en ajustant les paramètres de celui-ci afin d'appliquer l'algorithme d'évitement d'obstacles.
- Le controller permettant de suivre la lumière fonctionne selon la direction où la lumière est la plus forte - si celle-ci est plus forte en avant qu'en arrière, il avance et inversement. La même logique est appliquée pour les directions droite/gauche.
- La combinaison des deux algorithmes est faite en laissant principalement au controller, en charge du suivi de la lumière, le rôle d'influencer 
l'avancée / le reculé. L'évitement d'obstacles est quant à lui concentré sur la rotation, domaine où il a plus de poids que le controller de suivi de lumière puisque l'esquive d'obstacle est de nature plus urgente et critique.
- Nous avons dockerisé les deux controllers qui communiquent alors par des sockets avec le controller webots.
Ce dernier est en charge de fusionner les entrées des 2 controllers. Les deux controllers dockerisés sont des microservices - ils sont totalement indépendants et peuvent être deplacés vers d'autres applications ou interchangés.
- Un fichier docker-compose permet le lancement facile des différents conteneurs dockers avec leurs paramètres.

## Communication Docker-Webots:

Pour cette partie nous nous sommes inspirés du projet khepera-tcpip.

### Partie docker:

Les conteneurs Docker utilisent un réseau docker0 avec l'adresse host.docker.internal(adress de docker) qui est connecté au host.
Ainsi ils peuvent se connecter en tcp/ip vers host.docker.internal:10020 et host.docker.internal:10021.
Ils request les données de lumière et de distance des sensors du robot en envoyant L et renvoient avec R %fl %fl les double pour maitriser le mouvement du robot.
Pour envoyer et réceptionner les données nous utilisons send(fd, buffer, strlen(buffer), 0); et recv(fd, buffer, 256, 0);.
sscanf permet de distribuer les données depuis le buffer dans les variables.
sprintf permet de distribuer les données depuis les variables dans le buffer.

### Partie webots

Webots attend les connexions TCP en localhost sur 10020 puis 10021.
Des qu'ils sont connectés, le programme attend l'envoi des données. 
Lorsque les deux controllers ont envoyé des commandes, il recupère ces données, les assemble et bouge le robot puis renvoie les données des sensors.

## Conclusion

C'est un projet minimal viable qui prouve la praticapiliter de controler un robot webots avec deux micro-service dans des container docker.
A remarquer que webots et compatible avec windows linux et mac, de meme pour docker.
Donc nous avons une indépendance de l'os utiliser.

## Remarque
Il peut s'averer de sortir la logique de combainaison dans un autre docker.
Et donc construire un réseaux inter conteneur docker avec ces exitpoints qui communique avec webots plus que pour récupé des données et lancé les command robot.
Il est possible de conteneuriser webots mais celui étant une simulation graphique 3d
il risque dans ce cas avoir des performance problematique sachant que l'utilsation GPU est difficile à mettre en place et limiter sous docker.






