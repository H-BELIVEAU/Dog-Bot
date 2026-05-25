## Compte-Rendu Final  
**Auteur :** Hugo Béliveau  
**Classe :** 1ère année de classe préparatoire
**Ecole :** GRENOBLE INP - ESISAR
**Date :** Mai 2026  
**Projet**: [GitHub](https://github.com/H-BELIVEAU/Dog-Bot)

---

# Résumé du projet

Ce projet consiste à concevoir et réaliser un robot quadrupède piloté à distance, capable d’exécuter plusieurs positions fixes et animations simples.  
Le système repose sur une architecture distribuée : un PC avec interface Python, un Arduino UNO pour la communication RF, et un ESP32 embarqué sur le robot pour le contrôle des servomoteurs via un PCA9685.  
Le robot est alimenté par une batterie externe via un UBEC 5V 30A, et sa mécanique est imprimée en PLA.

---

# 1. Cahier des charges

## 1.1 Objectif général
Concevoir un robot quadrupède autonome, piloté à distance, capable d’exécuter des positions fixes et des animations.

## 1.2 Fonctionnalités attendues
- Positions fixes : debout, assis, couché, lever une patte  
- Animations : marche simple, danse, rotation sur lui-même  
- Pilotage en temps réel depuis un PC  
- Communication RF bidirectionnelle  
- Architecture évolutive pour ajout futur de capteurs  

## 1.3 Contraintes
- Budget limité (~100€)
- Utilisation obligatoire d’un microcontrôleur simple (Arduino/ESP32)  
- Alimentation stable et autonome
- Mécanique imprimée en PLA
- Temps limité (5 semaines)
- Communication entre PC et robot à distance

## 1.4 Entrées / Sorties
### Entrées
- Commandes RF provenant du PC  
- Architecture prévue pour l'ajout de futurs capteurs

### Sorties
- 12 servomoteurs MG996R  
- LED d’état sur ESP32  
- Retour d’état via RF vers le PC

## 1.5 Inspirations
![[imgs/BostonDynamics.png]]
	 [Chien robot de Boston Dynamics (qgmagazine.fr)](https://www.gqmagazine.fr/lifestyle/article/le-chien-robot-arrive-bientot))

![[imgs/TazerEngineering.png]]
	 [TazeEngineering (youtube.com)](https://www.youtube.com/@TazerEngineering)

---

# 2. Architecture matérielle

## 2.1 Microcontrôleurs
### Arduino UNO (côté PC)

L'Arduino UNO joue le rôle d'interface entre le PC et le robot. Il est branché en USB et communique via Serial avec le PC, et un module NRF24L01 branché par SPI ([[#2.2 Communication RF|voir 2.2]]) lui permet de communiquer par radio fréquence avec l'ESP32.

La liaison Serial se repose sur le protocole UART (Universal Asynchronous Receiver Transmitter), qui envoie les données bit par bit selon un format standard.
Le PC va transmettre les données via USB, l'Arduino UNO convertit l'USB en UART avec une puce ATmega16U2, qui communique à son tour avec le microcontrôleur ATmega328P via UART.

Les niveaux de tension sont de type TTL soit 0V pour le logique bas et +5V pour le niveau logique haut. Les vitesses de transmissions sont normalisées, souvent 9600 ou 11520 baud (symbole par seconde). Dans une connexion UART, on relie un GND commun, un TX de l'un qui vers le RX de l'autre, et inversement. 
Au repos, la ligne TX sera à l'état haut. Avant d'envoyer un message, TX passe à l'état bas (bit de start), puis à l'état des bits à transmettre (bits de données, généralement 8, pour un octet), puis se remet à l'état haut (bit de stop), et puis reste à l'état de repos. Le récepteur RX détecte le front descendant du bit de start, et capture à intervalle régulier (selon la vitesse de transmission configurée) les bits de données. 
Pour vérifier simplement si les données n'ont pas été corrompues pendant la transmission, on peut aussi ajouter au protocole un bit de parité.
Pour finir, bien que les deux lignes (TXA-RXB et TXB-RXA) soient indépendantes et peuvent fonctionner en parallèle, elles doivent partager la même vitesse de transmission.

### ESP32 (côté robot)
L'ESP32 constitue le cerveau du robot, qui va relier les actionneurs, le modules de communication et les futurs capteurs.
Il communique avec l'Arduino UNO par radio fréquence via un module NRF24L01 branché en SPI ([[#2.2 Communication RF|voir 2.2]]), et est relié par I2C à un PCA9685 ([[#2.3 Actionneurs|voir 2.3]]), le module qui pilote précisément chaque servomoteur en parallèles.

L'ESP32 a été choisis pour plusieurs raisons :
- Sa puissance de calcul, nécessaire pour la cinématique directe et inverse*
- Sa rapidité d'exécution
- Sa capacité à gérer plusieurs tâches
- Sa compatibilité avec l'environnement Arduino (notamment l'IDE Arduino)
- Son régulateur de tension 3.3V**

\*Pour l'instant, le PC réalise directement les calculs comme le robot est piloté et envoie directement les bonnes valeurs pour chaque servomoteur, mais la puissance de calcul est utile dans l'optique d'un mode autonome futur
\*\*En effet, le PCA9685 et l'antenne NRF24L01 doivent être alimentés en 3.3V

## 2.2 Communication RF

La communication entre le PC et le robot repose sur deux modules NRF24L01 PA LNA, donc équipés d'une antenne externe. Ces modules fonctionnent en 2.4GHz, fréquence autorisée en Europe, et notamment utilisée pour le Wifi, le Bluetooth, ZigBee ou encore ANT+. Pour stabiliser leur alimentation, un condensateur de 47µF a été ajouté proche des bornes VCC (3.3V) et GND.

Le pin CE (Chip Enable) active le mode TX ou RX (transmission/réception), car il ne peut pas envoyer et recevoir des données en même temps. Pour pouvoir avoir une impression qu'il puisse le faire, nous alternerons plusieurs fois par secondes les modes, pour pouvoir à la fois piloter le robot, mais également pouvoir recevoir des données des capteurs. Dans tous les cas, on a quand même la réception d'un ACK pour pouvoir vérifier si un paquet a bien été reçu. ([[#3.1 Interface PC (Python)|voir 3.1]]).

Le pin IRQ sert à prévenir lorsqu'un événement (paquet reçu, ACK reçu, échec après plusieurs tentatives) arrive via une interruption matérielle, mais nous ne l'utilisons ni pour l'Arduino UNO, ni pour l'ESP32. On peut tout de même recevoir les ACK et savoir si un problème est arrivé via le code, ça sera à une échelle plus lente, mais largement suffisante pour le projet.

Pour le reste, le module est branché via SPI (Serial Peripheral Interface) selon ce tableau :

| NRF24L01   | Arduino UNO | ESP32 (via VSPI) |
| ---------- | ----------- | :--------------: |
| MOSI       | 11          |     GPIO 23      |
| MISO       | 12          |     GPIO 19      |
| SCK        | 13          |     GPIO 18      |
| SS (ou CS) | 10          |      GPIO 5      |

Le protocole SPI est un protocole synchrone utilisé pour communiquer avec un, ou plusieurs périphériques rapidement.
Il est composé de 4 connexions :
- MOSI (ou COPI) : Master Out Slave In (ou Controller Out Peripheral In)
→ La ligne pour envoyer des données du contrôleur vers le périphérique
- MISO (ou COPI) : Master In Slave Out (ou Controller In Peripheral Out)
→ La ligne pour envoyer des données du périphérique vers le contrôleur
- SS (ou CS) : Slave Select pin (out Chip select pin)
→ La ligne utilisée par le contrôleur pour sélectionner le périphérique*
- SCK : Serial Clock
→ La ligne par laquelle l'horloge synchronise les transferts de données

\*Ce qui permet d'avoir plusieurs périphériques partageant les mêmes pins SPI sur MISO, MOSI et SCK. SS à LOW signifie que le périphérique est sélectionné.

Le protocole SPI est avantageux car il est très rapide (peur aller jusqu'à plusieurs MHz), full-duplex (MISO et MOSI fonctionnent en même temps), mais demande beaucoup de fils et 

## 2.3 Contrôleur PWM 16 canaux

Le module PCA 9685 est un contrôleur PWM 16 canaux, piloté en I2C, et alimenté logiquement en +3.3V via VCC et possède une alimentation externe via V+ ([[#2.5 Alimentation|voir 2.5]]) pour alimenter en puissance des actionneurs, avec un condensateur de 1000µF intégré pour absorber les pics de courants.
Il possède un oscillateur interne, ainsi que des registres pour chaque canal, qui permet de générer en sortie jusqu'à 16 signaux PWM indépendant et configurables avec une résolution de 12 bits (4096 pas), entre 40Hz et 1,5kHz, qui est d'ailleurs réduite pour être plus précis par un registre nommé PRE_SCALE.

Un signal PWM (Pulse Width Modulation) est un signal carré, dans lequel on va faire varier la largeur d'une impulsion (entre 1ms et 2ms) tout en gardant une période fixe. C'est utilisé pour réguler l'intensité d'une LED ou d'un ventilateur, en allumant et éteignant continuellement, ce qui est beaucoup plus simple que de réguler une tension, mais également pour le contrôle d'un servomoteur, sur son pin de donnée, qui indiquera l'angle auquel se placer ([[#2.4 Actionneurs|voir 2.4]]). 

Une connexion I2C est composée de deux lignes bidirectionnelles (et d'une masse commune) :
- SDA pour les données (Serial Data line)
- SCL pour l'horloge (Serial Clock Line)

Chaque périphérique I2C possède une adresse, donc il est possible brancher plusieurs modules sur le même bus. 
Le protocole fonctionne de cette manière :
- Le maître :
	- génère l'horloge
	- initie les communications
	- choisit quel esclave écouter ou commander
- Les esclaves :
	- Attendent qu'on les appelle
	- Répondent uniquement à leur adresse I2C

Et communiquent de la manière suivante :
	1. Le maître envoie un start
	2. Envoie l'adresse de l'esclave
	3. Indique s'il veut lire ou écrire
	4. L'esclave répond par un ACK
	5. Le maître envoie ou lit les données
	6. Le maître envoie un stop

Comme la communication SPI, l'I2C possède un protocole synchrone, avec une organisation de maître/esclave, et est supporté nativement par l'ESP32 et l'Arduino UNO. L'avantage de l'I2C est qu'il a moins de fils, avec un adressage intégré, mais est cependant plus lent (environ 100kHz, maximum 1MHz), plus sensible au bruit électrique. C'est pour cela qu'on préfère l'I2C pour les capteurs, drivers et modules nécessitant peu de bande passante, et SPI pour les modules RF, écrans TFT/OLED, ou encore modules de cartes SD, nécessitant un débit élevé.

Pin compatibles par défaut avec l'I2C sur ESP32 et Arduino UNO :

| I2C | Arduino UNO |  ESP32  |
| --- | :---------: | :-----: |
| SDA |     A4      | GPIO 21 |
| SCL |     A5      | GPIO 22 |


## 2.4 Actionneurs

Les actionneurs du robot sont exclusivement des servomoteurs MG996R, des servos à rotation limitée à environ 180°, qui possèdent un couple élevé (autour des 10kg.cm à 5V). Ils seront tous pilotés par le même module PCA9685 via un signal PWM ([[#2.3 Contrôleur PWM 16 canaux|voir 2.3]]), et alimentés en puissance par une source externe entre 5V et 6V.

Un servomoteur est un système composé de trois éléments :
- Un moteur DC
- Un train d'engrenage réducteur
- Un circuit de contrôle interne avec potentiomètre de retour
Un moteur entraîne les engrenages, qui réduisent la vitesse et augmentent le couple. Le potentiomètre est relié à l'axe de sorte, et mesure en permanence la position réelle du servo. Le circuit compare ainsi la position demandée via PWM et la position réelle, et ajuste le moteur pour corriger l'erreur.

Le MG996R utilise un signal PWM à 50Hz (période de 20ms). La largeur de l'impulsion détermine la position :
- 1ms → position minimale (~0°)
- 1,5ms → position centrale (~90°)
- 2ms → position maximale (~180°)
Donc le signal PWM n'évolue pas linéairement par rapport à la position demandée.

Caractéristiques du MG996R :
- Tension : 4,8V à 7,2V
- Courant à vide : ~120mA
- Courant en charge : ~600mA
- Courant de blocage : 2,2A (d'où la nécessité d'une alimentation puissante, [[#2.5 Alimentation|voir 2.2]])
- Couple : 9,4 kg.cm à 4,8V, 11 kg.cm à 6V
- Vitesse (à vide) : 0,17s /60° à 4,8V, 0,14s /60° à 6V
- Rotation : ~180°
- Poids : ~55g (avec engrenages en métal)

Ce modèle de servomoteur est également un bon choix pour le projet de part son faible coût, qui revient à moins de 50€ pour 12 exemplaires.

## 2.5 Alimentation

L'alimentation sur le robot est organisée en deux branches distinctes, avec une branche puissance en 5V pour les servomoteurs et l'entrée de l'ESP32, et une branche logique en 3.3V (fournit par l'ESP32) pour l'électronique de commande, avec toutes les masses reliées ensemble.

La batterie - actuellement une batterie de trottinette électrique récupérée, surdimensionnée mais pratique en raison d'un retard de livraison, est connectée via un connecteur XT60 à un interrupteur haute tension, puis à un UBEC qui abaisse la tension et fournit une sortie de 5V pour un maximum de 15A. La batterie intégrée au robot sera une batterie Lipo 3300mAh.

## 2.6 Mécanique

La structure du robot est majoritairement imprimée en PLA (léger, rigide et simple à imprimer)
Pour la plaque centrale, un support en carton sera utilisé pour économiser du temps d'impression (notamment en raison de la panne de mon imprimante 3D, [[#5. Difficultés rencontrées|voir 5]]) et du matériau.
Le design a été pensé pour être simple à assembler, léger, mais suffisamment rigide pour supporter les efforts transmis par les servomoteurs MG996R, et le mécanisme de la patte a été inspiré du Youtubeur TazerEngineering ([[#1.5 Inspirations|voir 1.5]]), mais une version repensée pour être entièrement imprimée en PLA.

Chaque patte est entièrement démontable, grâce à l'utilisation de vis et de logements dédiés. 
Cette modularité permet de :
- remplacer rapidement un servomoteur défectueux/pièce cassée
- de tester différentes géométries de pattes
- de modifier la cinématique sans réimprimer tout le robot
- de faciliter le transport

Chaque patte est composée de trois servomoteurs MG996R, deux sur un même axe (coude et cheville), et un qui met en rotation cet axe (épaule). Les trois sont regroupés au même endroit, au niveau de l'articulation supérieure.
Ce choix inspiré de TazerEngineering présente l'avantage de réduire les masses en mouvements, donc le couple nécessaire, et me permet donc d'obtenir une meilleure précision, moins de vibrations, mais également de meilleures protections des servomoteurs, qui se trouvent donc près du corps.

Nous verrons plus tard plus en détail la modélisation de la patte comme système ([[#4. Modélisation et cinématique|voir 4]]).

Les moteurs sont fixés par des vis, mais les palonniers (servo hors) sont eux seulement emboîtés, pour éviter la casse. En cas de surcharge d'un servomoteur, le plastique cèdera avant les engrenages en métal. Les liaisons pivot de la patte sont eux simplement imprimés en 3D, poncés puis collés avec des chapes pour les empêcher de sortir. 

Le PCB ([[#2.7 Circuit Imprimé|voir 2.7]]) sera quant à lui fixé à une plaque imprimée en PLA, qui sera vissée au corps du robot.

En terme de quantité d'impression, une patte complète représente environ 1h30 d'impression avec l'imprimante Anycubic Kobra S1, pour environ 50g de PLA, ce qui représente pour le robot entier environ 7h d'impression pour 230g de PLA, en incluant la plaque PCB.

Avec la masse mesurée des servomoteurs, ainsi que la masse du carton, nous arrivons à un total d'environ 1,7kg et de 400g par pattes. Pour des articulations d'environ 15cm, c'est largement bon pour les épaules et coudes, mais risque d'être un peu plus critique sur l'épaule. ([[#4.4|voir 4.4]] pour les calculs et [[#6. Résultats obtenus|voir 6]] pour les résultats obtenus).

Toutes les pièces ont été modélisées sur le logiciel Fusion 360 ([[#8.3 Modélisation 3D|voir 8.3]]).

## 2.7 Circuit Imprimé

Pour fiabiliser les connexions (réduire le risque de faux contacts avec les câbles Dupont), et rendre le robot plus propre et plus compact, j'ai décidé de passer à une version soudée. J'ai d'abord tenté d'utiliser une plaque à trous ([[#5. Difficultés rencontrées|voir 5]]), mais je suis ensuite passé sur un circuit imprimé.

L'objectif est de relier les composants, en utilisant des connecteurs pour facilement pouvoir retirer les composants, et pour l'alimentation de l'UBEC. Seulement le condensateur prévu pour le module NRF24L01 est soudé directement au PCB. J'ai également percé quelques trous pour pouvoir fixer le circuit au robot avec une pièce en PLA.

Le circuit imprimé a été modélisé sur KiCad, un logiciel gratuit et open source permettant de créer des schémas et des designs de PCB. J’ai d’abord récupéré les empreintes de l’ESP32, du module NRF24L01 et du PCA9685, puis relié l’ensemble selon mon schéma électrique ([[#8.2 Schémas électroniques|voir 8.2]]).

Je suis ensuite passé à l’organisation des composants et au routage. J’ai cherché à réduire au maximum la taille du PCB, mais avec le recul, le module NRF24L01 est peut‑être trop proche de l’antenne de l’ESP32, ce qui pourrait poser des problèmes radio ([[#7. Perspectives|voir 7]]). J’ai utilisé un plan de masse pour simplifier le routage, améliorer la distribution du GND et éviter de retirer tout le cuivre de la plaque. Des vias ont été placés pour relier les différents plans de masse. J’ai également veillé à ce que les connexions vers les connecteurs soient routées sur la face inférieure pour faciliter la soudure, à ne pas placer de plan de masse sous les antennes, et à orienter l’antenne du NRF24L01 vers l’extérieur, ainsi que le port USB‑C de l’ESP32 et les connecteurs des servomoteurs du PCA9685.

La largeur des piste est assez large (1,5mm) pour toutes les pistes, de même pour les vias (2,5mm de diamètre).

Ensuite, une fois imprimé, j'ai percé tous les trous, soudés mes vias, mes connecteurs, et finalement vérifié la continuité entre chaque connexion. J'ai d'ailleurs trouvé à ce moment là qu'il me manquait une connexion pour pouvoir brancher le PCA9685 dans les deux sens, mais corrigé facilement en soudant un câble ([[#5. Difficultés rencontrées|voir 5]]). Malgré ça, le circuit imprimé a fonctionné parfaitement du premier coup.  

---

# 3. Architecture logicielle

## 3.1 Interface PC

L'interface PC constitue la partie logicielle permettant de contrôler le robot depuis un ordinateur. Elle est développée en Python et regroupe une interface graphique avec visualisation de la position de la patte et l'envoie de commandes à distance avec la communication série avec l'Arduino UNO.

J'ai décidé de séparer le code en deux parties : Interface graphique, communication Serial et gestion d'une manette.

En premier, l'interface est générée par la bibliothèque [PyQt6]([PyQt6 · PyPI](https://pypi.org/project/PyQt6/)), qui permet de créer des applications Python assez facilement. La bibliothèque gère les événements (clic sur un bouton, touche pressée, mise à jour de la visualisation, données reçue) par des signaux, qui permettent de relier l'événement à une fonction. 
Une interface PyQt6 a une architecture d'arbre, avec un élément principal qui est la fenêtre, puis une division en plusieurs sections, et enfin des éléments. De plus, les éléments peuvent être stylisés par l'utilisation de CSS. Egalement, PyQt6 fonctionne avec un thread principal, donc la communication série ou avec la manette doit être non bloquante, mais on peut gérer ça avec un timer pour lire le Serial sans bloquer l'interface.

Ensuite, la communication Serial est gérée par la bibliothèque [pyserial]([pyserial · PyPI](https://pypi.org/project/pyserial/)). Avec celle-ci on peut à la fois envoyer des données, mais aussi en recevoir très facilement. J'ai ajouté ça une gestion automatique de changement de port, et relié avec l'interface graphique.

Finalement pour la manette, j'utilise [pygame]([pygame · PyPI](https://pypi.org/project/pygame/)) qui me permet très facilement d'interfacer une manette à mon programme.

Avec ce mélange, j'ai pu créer un programme qui contient les fonctions suivantes :
- Boutons de commandes principales (Reset, Connect Serial, Connect Controller)
- Informations principales (Ping, Périphériques connectés)
- Stockage des données partagées avec le robot (angles des servomoteurs)
- Visualisation de position d'une patte (avec cinématique directe)
- Contrôle de la patte (par servomoteur et position, [[#4. Modélisation et cinématique|voir 4]])
- Gestion d'animations (marche, dance, patte en l'air, idle, assis et couché)
- Affichage du Serial en direct
- Possibilité d'envoyer des commandes directement sur le Serial
- Envoie des instructions pour les angles à chaque modification

Pour l'instant, le robot n'est pas autonome au niveau logiciel, déjà car il n'a pas de capteurs intégré. Il est entièrement piloté à distance, avec l'envoie continu des instructions pour chaque servomoteur individuellement.
Alors à chaque fois qu'une valeur d'angle de servomoteur est mise à jour par un événement (contrôle manette, animation, etc.), on envoie à l'Arduino UNO l'instruction via le Serial. 

## 3.2 Arduino UNO

L'Arduino UNO joue le rôle d'interface entre le PC et le robot. Il reçoit les commandes envoyées par l'interface Python via USB, les convertit en paquets binaires adaptés au module NRF24L01, puis les transmet au robot. Il gère également les accusés de réception (ACK), renvoie les informations reçues vers le PC, et gère de façon autonome le changement de mode du module NRF24L01 pour échanger dans les deux sens avec le robot.

Premièrement, j'ai mis en place avec mon programme PC un petit protocole assez simple pour l'échange d'information, afin que ce système d'Arduino UNO, avec connexion PC Serial et microcontrôleur par RF soit réutilisable dans d'autres projets, sans avoir à recoder tout.
Lorsqu'il reçoit une donnée via le Serial, elle sera de la forme : "KEY:TYPE:DATA", voici des exemples :
Si je veux envoyer un commande pour bouger le servo 5 à 60°, j'envoie par exemple : "Servo_5:INT:60". Il décode ainsi grâce aux ":" les informations, les place dans des paquets, et les envoies. Il peut également envoyer des nombres flottants, et des listes d'entiers ou de flottants.
INT correspond au type int32, FLOAT au type float, TABINT à un tableau d'entier int32, TABFLOAT à un tableau de nombres décimaux float, et STRING à une chaîne de caractère char.
Les nombres décimaux séparent leur partie entière et leur partie décimale par un point ".", les tableaux séparent leur éléments par des virgules ",", et finalement le premier élément de chaque tableau doit contenir le nombre exact d'éléments dans la liste, lui-même non compris.

Lorsqu'il reçoit une information, il réalise la même tâche à l'inverse, transformant le paquet reçu en chaine de caractère qui sera interprétable par le PC.

Il ne réalise donc aucune tâche de contrôle du robot, juste de la transmission de données.

Pour communiquer en Serial, on utilise la [bibliothèque d'Arduino]([Serial | Arduino Documentation](https://docs.arduino.cc/language-reference/en/functions/communication/serial/)), qui permet de lancer le Serial, vérifier s'il fonctionne, vérifier si des données sont reçues, et en envoyer.

Pour communiquer en SPI avec le NRF24L01, on utilise les bibliothèques [SPI]([Extended SPI Library Usage with the Arduino Due | Arduino Documentation](https://docs.arduino.cc/tutorials/due/due-extended-spi/)) et [RF24]([RF24 | Arduino Documentation](https://docs.arduino.cc/libraries/rf24/)), l'une permettant de gérer la communication avec le module, et la seconde permettant de contrôle et réception de données du NRF24L01.
Dans le programme, on commence par instancier le module RF en précisant les pins CE et CS ([[#2.2 Communication RF|voir 2.2]]), et en définissant un tableau des adresses de tunnel, qui doivent être communes aux deux programmes communiquant par module RF. On en choisis une dans un sens, et une seconde pour l'autre sens.
Ensuite, on initialise le module NRF24, on ouvre un chanel en écriture, l'autre en lecture, et enfin on sélectionne un niveau de puissance pour communiquer. Ici on reste très proche du robot, donc on reste au niveau minimal (RF24_PA_MIN).
Dans la boucle, on commence par arrêter le mode d'écoute, pour pouvoir émettre les données. Une fois envoyé, on place une très légère pause (environ 5ms), puis on arrête le mode d'envoie, pour vérifier maintenant si des données sont reçues. Si c'est le cas, on commence la lecture de chaque donnée, une par une. On place ensuite une seconde légère pause, avant de recommencer la boucle.

Pour transmettre ou recevoir une donnée via RF, on doit définir une structure, qui permettra de convertir nos données en paquets. Pour simplifier je vais garder la chaîne de caractère comme structure de données.
La clé de donnée aura une taille maximale de 12 caractères, le type de donnée une taille maximale de 8 caractères, et enfin la donnée en elle même aura une taille maximale de 490 caractères. En comptant les séparateurs ":", on obtient un maximum de 512, donc un total de 512 octets par paquets. 

## 3.3 ESP32

De la même manière que l'Arduino UNO, l'ESP32 échange des données via le NRF24L01 ([[#3.2 Arduino UNO|voir 3.3]]).

Pour cette première version du robot, dans laquelle il n'est pas autonome et ne possède pas de capteurs, il ne fait que piloter le PCA9685, donc le code sur cette partie est assez léger.

Pour pouvoir communiquer avec le PCA9685, l'ESP32 utilise la bibliothèque [Adafruit PWM Servo Driver]([Adafruit PWM Servo Driver Library | Arduino Documentation](https://docs.arduino.cc/libraries/adafruit-pwm-servo-driver-library/)). Au début du programme, nous devons créer l'objet représentant le PCA9685 en spécifiant l'adresse à utiliser (0x40 sur l'ESP32 pour utiliser l'adresse I2C par défaut) et l'initialiser. Ensuite, nous utiliserons seulement une seule méthode : "setPWM(channel, on, off)".
Channel représente le canal à sélectionner (de 0 à 15), on le tick (entre 0 et 4095) où le signal doit transitionner de l'état bas vers l'état haut, et off le tick où il soit revenir à l'état bas.
Pour faire simple, on va garder le on sur 0, et seulement manipuler la valeur de off. Par exemple, off à 0 signifie que la sortie sera toujours basse, off à 4095 signifie que la sortie sera toujours haute, et off à 2044 signifie que la sortie sera basse sur 50%, puis haute sur 50%.

Après de court tests, j'ai pu déterminer les valeurs PWM (tick pour off) à mettre pour avoir un angle sur chaque borne (0° et 180°), que je nomme SERVO_MIN_PWM et SERVO_MAX_PWM.
Lorsqu'il reçoit un paquet, il s'agira pour l'instant toujours d'une instruction d'angle pour un servomoteur. Les clés sont donc du format : "SERVO_X", avec X l'index du servomoteurs sur le canal du PCA9685, avec en donnée un entier entre 0 et 180. 
J'étalonne ensuite la valeur entre 0 et 4015 à l'aide de la fonction "map" d'Arduino, et j'envoie l'instruction au PCA9685.

---

# 4. Modélisation et cinématique

Dans cette partie, l'objectif est de pouvoir déterminer à l'avance comment contrôler le pied, et pour cela nous devons commencer par le modéliser.

Commençons par le plan formé par les servomoteurs représentant à deux le coude et la cheville de la patte.
On choisis de se placer sur le repère orthonormé définit le point formé par l'axe du premier servomoteur comme l'origine $O$, et par l'orientation du corps du robot.
Ainsi le second servomoteurs est caractérisé par le point $A(ax,ay)$, où $ax$ et $ay$ sont deux paramètres. On choisis ensuite de caractériser les angles des deux servos $\Theta_1$ et $\Theta_2$ par rapport à l'axe horizontal dans le sens trigo. Finalement, on forme deux parallélogrammes $OABC$ et $ODEF$, reliés avec un triangle $OCD$, et un place le point de la patte $M$ sur la droite $(EF)$.
Les barres $OF$ et $AB$ sont chacun fixé aux servomoteurs, et toutes les autres liaisons forment des liaisons pivots.
Les longueurs $AB$, $OC$, $OD$, $EF$ sont égales et valent la longueur $L1$.
Les longueurs $OF$, $DE$ et $FM$ sont égales et valent la longueur $L2$.
Finalement, l'angle du triangle $\angle{DOC}$ est égal au paramètre $\alpha$.

Schéma de la modélisation du système formé par le plan cheville/coude :
XXX

Ensuite, ce plan est mis en rotation (épaule) sur l'axe horizontal passant par le servomoteur caractérisé par le point $A$.

## 4.1 Cinématique directe

En restant simplement sur le plan coude/cheville :

Données connues : $O(0; 0)$, $A(ax, ay)$, $L1$, $L2$, $\alpha$

$C = (L1.cos(\Theta_2),\ L1.sin(\Theta_2))$
$B = A+C = (ax+L1.cos(\Theta_2),\ ay+L1.sin(\Theta_2))$
$D = (L1.cos(\Theta_2+\alpha),\ L1.sin(\Theta_2+\alpha))$
$F = (L2.cos(\Theta_1),\ L2.sin(\Theta_1))$
$E = D+F = (L1.cos(\Theta_2+\alpha)+L2.cos(\Theta_1),\ L1.sin(\Theta_2+\alpha)+L2.sin(\Theta_1))$

Et enfin la caractérisation du pied :
$M = F+\vec{EF}.\frac{L2}{L1} = F - D\frac{L2}{L1}=(L2.cos(\Theta_1)-L2.cos(Theta_2+\alpha),\ L2.sin(\Theta_1)-L2.sin(Theta_2+\alpha))$

## 4.2 Cinématique inverse

Cette fois, l'objectif est de réussir à déterminer quels angles mettre sur chaque servomoteurs pour que le pied se trouve en position $M(x,y)$.

XXX

## 4.3 Utilisation pour les animations

Maintenant j'ai pu utiliser les résultats obtenus pour générer des animations, et des positions fixes.

Par exemple, pour l'animation de marche, j'ai cherché à avoir deux phases, une première avec une ligne droite qui est la phase d'appuie, et ensuite un arc de cercle qui est le phase de retour. J'ai fait en sorte qu'il y ait plus d'appuie que de retour pour maintenir au plus l'équilibre.
Une fois que j'ai modélisé le déplacement de la patte, je convertit la position de patte en angles de servomoteurs grâce à ma cinématique inverse, et les stocker dans un tableau dédié.
Egalement, j'utilise la cinématique directe pour visualiser directement la patte dans l'interface ([[#3.1 Interface PC|voir 3.1]]).

---

# 5. Difficultés rencontrées

Ma première difficulté rencontrée, a été la soudure avec une plaque à trous, qui ne tenait pas, avait des problèmes de continuité, des fils qui perdaient seuls jusqu'à 2V, et un résultat vraiment pas propre. Heureusement, j'ai eu accès à la réalisation d'un circuit imprimé.

Ensuite, j'ai eu un module NRF qui ne fonctionnait plus, et qui chauffait très fort. Je soupçonne que ça vienne d'un court circuit à cause des problèmes de continuité précédents. J'ai pu le remplacer rapidement, et ça a fonctionné à nouveau.

Pour la suite, la livraison de la batterie Lipo a pris énormément de retard, et est prévue pour plus d'un mois après la date à laquelle nous devons rendre le projet. Heureusement, j'ai trouvé une batterie de trottinette électrique qui fonctionne tout aussi bien, qui me permet de faire mes tests, mais qui est beaucoup trop grande et lourde pour le robot.

Le plus gros problème du projet a été une panne d'imprimante 3D. Le temps de contacter le service après vente, qu'ils me renvoient les pièces en que tout fonctionne, j'étais à deux jours de devoir présenter le projet, ce qui ne m'a pas laissé la place à l'exploration de nouveaux designs, ou de prototypes plus poussés.

---

# 6. Résultats obtenus

A la date de présentation du projet, je n'ai pas pu réaliser entièrement l'assemblage, surtout en raison de la panne d'imprimante 3D, mais j'ai avancé énormément sur ce qui est une expérimentation et prototypage d'un chien robotique.

J'ai un PCB complètement fonctionnel, et qui me permet de facilement remplacer mes composants, une interface réutilisable entre contrôle sur PC et communication par radio fréquence et Serial via USB par un Arduino UNO, une modélisation cinématique directe et inverse opérationnelles sur une patte, une communication par radio fréquence bidirectionnelle, une architecture prête pour 4 pattes, une interface graphique sur PC, et surtout, une patte de chien robot entièrement fonctionnelle [[#8.1 Photos du prototype|voir 8.1]]. 

Ce qu'il me manquerait à faire, c'est d'imprimer 4 pattes différentes, et d'assembler le tout pour former un corps. 

---

# 7. Perspectives

Le projet s'ouvre à énormément d'améliorations futures, en commençant par l'ajout de capteurs (IMU pour stabilisation, Ultrasonique pour distances), des animations plus fluides, en évitant les changements de directions trop secs, un mode de déplacement autonome, une version plus fiable mécanique, avec des liaisons métalliques et toutes les fixations réalisées avec des vis.

Une autre amélioration serait de choisir des servomoteurs plus puissants sur les épaules, pour pouvoir supporter de plus grandes charges.

---

# 8. Annexes

## 8.1 Photos du prototype  
*(Insérer images)*

## 8.2 Schémas électroniques  

Schéma des connexions électroniques :
![[imgs/ElectricSchematic.png]]

Circuit imprimé : (face haute puis face cachée)
![[imgs/CuivreF.png]]
![[imgs/CuivreB.png]]
(à noter qu'un fil a été rajouté, pour relier les deux SCL du PCA9685 qui n'étaient finalement pas reliés à l'intérieur du module)

## 8.3 Modélisation 3D  
*(Captures d’écran)*

## 8.4 Extraits de code  

Code Python qui utilise le Serial :
```
class SerialWorker(QObject):  
    received = pyqtSignal(str)  
    connected = pyqtSignal(str)  
    disconnected = pyqtSignal()  
    error = pyqtSignal(str)  
  
    def __init__(self):  
        super().__init__()  
        self.port = QSerialPort()  
        self.port.readyRead.connect(self.read_data)  
  
    @pyqtSlot(str, int)  
    def open(self, port_name, baudrate):  
        self.port.setPortName(port_name)  
        self.port.setBaudRate(baudrate)  
  
        if self.port.open(QSerialPort.OpenModeFlag.ReadWrite):  
            self.connected.emit(port_name)  
        else:  
            self.error.emit("Impossible d'ouvrir le port")  
  
    @pyqtSlot()  
    def close(self):  
        if self.port.isOpen():  
            self.port.close()  
            self.disconnected.emit()  
  
    @pyqtSlot(str)  
    def write(self, data):  
        if self.port.isOpen():  
            self.port.write((data + "\n").encode("utf-8"))  
  
    def read_data(self):  
        while self.port.canReadLine():  
            line = bytes(self.port.readLine()).decode("utf-8").strip()  
            self.received.emit(line)  
  
  
class SerialManager:  
    def __init__(self):  
        self.thread = QThread()  
        self.worker = SerialWorker()  
        self.worker.moveToThread(self.thread)  
        self.thread.start()  
  
    def list_ports(self):  
        return [p.portName() for p in QSerialPortInfo.availablePorts()]  
  
    def connect(self, port, baud=115200):  
        QMetaObject.invokeMethod(self.worker, "open", Qt.ConnectionType.QueuedConnection, Q_ARG(str, port), Q_ARG(int, baud))  
  
    def disconnect(self):  
        QMetaObject.invokeMethod(self.worker, "close", Qt.ConnectionType.QueuedConnection)  
  
    def send(self, text):  
        QMetaObject.invokeMethod(self.worker, "write", Qt.ConnectionType.QueuedConnection, Q_ARG(str, text))
```

Code sur l'Arduino UNO, qui interface le PC avec Serial via USB au robot avec RF via SPI :
```
XXX
```

Code sur l'ESP32, qui échange les données via RF et pilote les servomoteurs avec le PCA9685 via I2C :
```
XXX
```
