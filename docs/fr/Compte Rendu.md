Auteur : Hugo BELIVEAU
Ecole : GRENOBLE INP - ESISAR
Date : Mai 2026

## Descriptif du projet
---
Ce projet consiste à concevoir et réaliser un robot quadrupède piloté à distance, capable d’exécuter plusieurs positions fixes et animations simples.  
Le système repose sur une architecture distribuée : un PC avec interface Python, un Arduino UNO pour la communication RF, et un ESP32 embarqué sur le robot pour le contrôle des servomoteurs via un PCA9685.  
Le robot est alimenté par une batterie externe via un UBEC 5V 30A, et sa mécanique est imprimée en PLA et utilise également une base en bois, et quelques vis.

Il s'agit d'un projet de première année d'élec à l'ESISAR, choisis par l'élève, sur cinq séances étalés sur un mois, ayant comme principale obligation d'utiliser un microcontrôleur simple (gamme Arduino ou ESP32).

Ce fichier est mon compte rendu pour le projet, et est tenu dans un ordre chronologique. 
Pour avoir une documentation purement fonctionnelle, j'ai réalisé un autre fichier sans ordre chronologique, traitant seulement la partie fonctionnelle du projet : [[Projet Chien Robot]]. Egalement, le projet entier est disponible sur mon GitHub : [Dog Bot](https://github.com/H-BELIVEAU/Dog-Bot), et une vidéo YouTube est réalisée dessus [(En cours de réalisation..)]([Hugo Engineering - YouTube](https://www.youtube.com/@hugo_engineering)).

## Table des matières
---
[[#1. Cahier des charges]]
	[[#1.1 Objectif général]]
	[[#1.2 Fonctionnalités attendues]]
	[[#1.3 Contraintes]]
	[[#1.4 Entrées / Sorties]]


## 1. Cahier des charges
---
### 1.1 Objectif général
---
Concevoir un robot quadrupède autonome, piloté à distance, capable d’exécuter des positions fixes et des animations simples. 

### 1.2 Fonctionnalités attendues
---
On attend du robot qu'il puisse être piloté à distance depuis une interface sur un ordinateur, par le moyen de communication par radio fréquence bidirectionnelle.
Tout le projet devra avoir au maximum une architecture évolutive et réutilisable, pour permettre un ajout futur de capteurs additionnels, ou d'un mode autonome dans une prochaine version.

### 1.3 Contraintes
---
Le budget tout compris pour le robot est limité à une centaine d'euros.
Le microcontrôleur du système doit être un microcontrôleur simple (gamme Arduino ou ESP32).
L'alimentation doit être stable et autonome, pour pouvoir déplacer librement le robot.
La mécanique doit être imprimée en PLA, excepté pour les parties trop grandes et les zone à trop grande contraintes mécaniques. 
Il devra y avoir une communication à distance fiable entre le PC et le robot.
Le temps est limité à 5 semaines de projets, avec 5 séances en groupes de TP.

### 1.4 Entrées / Sorties
---
#### PC
- Echange des données dans les deux sens par un Serial commun à un Arduino UNO
- Affiche une interface de contrôle
#### Arduino UNO
- Reçoit les données du PC par Serial et les renvoies par radio fréquence vers l'ESP32
- De même dans le sens inverse
#### ESP32
- Echange des données avec l'Arduino UNO par radio fréquences
- Contrôle les servomoteurs avec un multiplexeur PWM communiquant par I2C

## 2. Première semaine
---

Lors de la première semaine, j'ai d'abord conçu mon cahier des charges, pour avoir un objectif plus clair en tête. 
Une fois que je savais à peu près ce que mon robot devrait faire, j'ai ensuite réfléchis aux composants qui me seront utiles, mais également à la mécanique derrière, pour tout choisir au mieux. Tout au long du projet, je vais faire au mieux pour rendre le projet modulable et ouvert à de futures améliorations.
Tout d'abord, pour le "cerveau" du robot, j'ai choisis un ESP32, car il reste assez simple d'utilisation, mais est beaucoup plus rapide qu'un Arduino UNO. Et comme je vais devoir piloter beaucoup d'actionneurs, et communiquer avec plusieurs modules en même temps, cette puissance de calcul sera utile, d'autant plus que si plus tard je souhaite faire un mode autonome, il me faudra encore plus de calculs réalisés.
Ensuite, pour faire bouger le robot, je vais choisir des servomoteurs. Encore une fois, ça reste assez simple d'utilisation, mais cette fois ça me limitera sur la précision et la puissance. Sur une patte, j'aimerai avoir une cheville, un coude et une épaule, alors il me faudra trois servomoteurs par pattes. Cependant, les coudes porteront tout le reste de la jambe, alors je vais choisir des MG996R pour les coude et les chevilles, et des MG946R sur les épaules car ils sont tous les deux assez puissants, mais le MG946R l'est encore plus.
Pour contrôler les 12 servomoteurs, je choisis d'utiliser un PCA9685, qui est un multiplexeur de signal PWM sur 16 canaux, qui possède en plus un pin V+ pour de la haute alimentation et des connecteurs pour brancher tous les servomoteurs.
Pour la communication à distance, il me restait des modules NRF24L01, qui communiquent par radio fréquences, ça me permettra du budget, mais aussi du temps de code. D'ailleurs pour la partie passerelle (gateway), j'ai simplement choisis un Arduino UNO que j'avais déjà, et un second module NRF24L01, qui ne seront pas soudés, et seulement utilisés le temps du projet.
Pour pouvoir alimenter tout le circuit, il me fallait une ligne avec beaucoup de puissance pour alimenter les servomoteurs, mais il fallait également qu'elle soit portable. Alors j'ai choisis de prendre une batterie Lipo en 7.3V, et un UBEC pour réguler la tension en 5V pour un maximum de 30A. 
Finalement, j'ai voulu tenter les planches à trous pour réaliser le circuit électrique plus tard, et j'ai acheté avec deux petites bobines de fils, une de largeur assez classique, et une autre pour de la plus haute puissance.

Avant d'acheter, j'ai réalisé le reste de cette semaine pas mal de schémas en tout genre pour vérifier au mieux que tout concordait, avec des schémas électriques, des schémas sur la mécanique de la patte, mais aussi des schémas sur l'architecture du code et de la communications.

## 3. Deuxième et troisième semaines
---
Lors de cette deuxième semaine, j'ai passé ma commande sur Ali express des composants qu'il me manquait :
- 1x Pile Lipo 3300mAh
- 1x PCA9685
- 8x MG996R
- 4x MG946R
- 1x UBEC 5V 30A
- 1x Adaptateur XT60 (pour relier le UBEC et la batterie)
- 1x chargeur de batterie
- 1x Bobine de fil 16AWG (câble large)
- 1x Bobine de fil 24AWG (câble classique/fin)
- 1x Planche de prototypage PCB à trous
Je n'ai plus la répartition exacte, mais la grosse majorité du budget passe assez logiquement dans les servomoteurs, puis dans la batterie et son chargeur.

Cette semaine ci, j'ai également commencé à modéliser le système de la patte, cinématique pour faire des simulations, mais également en 3D avant de l'imprimer.
La partie modélisation cinétique était facile dans le sens où l'on part des angles connus des servomoteurs pour déterminer les positions des autres points, mais beaucoup plus complexe sur la résolution inverse, où j'essaye de déterminer les angles des servomoteurs à mettre pour atteindre un point particulier. ([[XXX|Les détails sont traités ici]).
Et la partie modélisation 3D a été réalisée sur Fusion 360, dans l'objectif de pouvoir ensuite imprimer les pièces mécaniques en 3D.
J'ai réalisé les deux modélisations en parallèles, pour déterminer les valeurs de chaque longueurs, angles et positions de départs, tout en prenant en compte les bornes des servomoteurs, et tout ça a duré sur les deux semaines.

Sur la troisième semaine, en même temps que les simulations, j'ai pu déjà commencer à tester une version prototype de la patte, avec deux servomoteurs SG90, et les longueurs réduites. Avec cette version, je branchais directement l'alimentation des servomoteurs à un générateur, et je contrôlais les servomoteurs directement sur un Arduino UNO par PWM. J'ai pu tester d'abord la mécanique simple en contrôlant mes deux servomoteurs, et ensuite j'ai essayé ma cinématique inverse, et elle a fonctionnée du premier coup. 

## 4. Quatrième semaine
---
J'ai reçu la commande des composants le week-end même de la troisième semaine, alors j'ai pu commencé à tester un par un les composants. A priori, tout avais l'air de fonctionner. Le seul problème que j'ai eu à ce moment là, a été le gros retard de ma batterie Lipo, avec une livraison annoncée pour trois semaines après la date de présentation du projet. Heureusement, j'ai trouvé une batterie de trottinette électrique récupérée, qui m'a permis de pouvoir continuer le projet, bien qu'elle ne soit pas adaptée de par sa taille et son poids.

Une fois que tout a fonctionné, j'ai décidé de directement souder tous les composants sur la plaque de prototypage de PCB, pour ne plus avoir de problème de branchement sur les déplacements ou lorsque le microcontrôleur bouge un peu. Alors je suis passé à la soudure, et ça n'a vraiment pas été une réussite, entre les pastilles des trous qui s'enlèvent, les faux contacts, les fils qui perdaient en tensions, etc. 

En même temps que tout ça, j'ai commencé à coder l'interface Python, le code de l'ESP32 et le code de l'Arduino UNO pour pouvoir commander l'ESP32 à distance, directement depuis l'interface sur le PC. 

Pour finir, cette semaine, mon imprimante 3D s'est cassée, et j'ai donc directement contacté le support de la marque, pour qu'ils puissent me renvoyer les pièces au plus vite.

## 5. Cinquième et sixième semaines
---
J'ai eu le temps de finir presque entièrement la structure du code qu'il m'était possible de faire sans patte (car l'imprimante 3D est toujours cassée), mais pas possible d'avancer plus.

Là où j'ai le plus avancé, c'est sur le circuit, car j'ai eu accès à la réalisation d'un PCB directement à l'ESISAR. J'ai donc refait mon circuit électrique sur KiCad, j'ai pu l'avoir à la fin de la semaine, le percer, et commencer à souder les composants dessus. D'ailleurs, pour pouvoir faciliter le remplacement des modules, ils n'ont pas été directement soudés sur le circuit imprimé, mais simplement rentrés dans des connecteurs qui ont été eux-mêmes soudés. Et c'est de même pour l'alimentation, avec des connecteurs pour simplifier les déplacements. J'ai aussi fait attention à mettre toutes les soudures sur la face arrière, et j'ai découvert ce qu'était un plan de masse.

Au niveau de l'imprimante 3D, je n'ai toujours pas reçu les pièces cette semaine là, ni celle d'après. C'est à ce moment là que j'ai perdu le plus de temps sur le projet, car je ne pouvais plus rien avancer sans avoir une patte pour pouvoir réaliser des tests.

## Septième semaine
---
Dans la septième et dernière semaine, j'ai enfin reçu les pièces de remplacement, et par chance, le remplacement seul a suffit à faire refonctionner la machine. 
Cette semaine, j'ai passé toute mes journées, du matin au soir, à tester une patte, corriger une erreur, voir qu'un design n'allait pas, le corriger, en espérant rattraper le retard qui a été pris. 
J'ai également pu prendre le temps de réaliser la base du robot en bois, et un support pour le PCB.
Malheureusement, en allant trop rapidement, je n'ai pas pris mon temps, et j'ai cassé un des servomoteurs qui servait d'épaule (sûrement à cause de pics de tensions trop élevés, en tentant de faire bouger à haute vitesse trop de servomoteurs), et si proche de la fin du projet, il ne m'est pas possible de le remplacer, alors le projet ne pourra pas aboutir.