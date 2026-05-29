
**Auteur :** Hugo Béliveau  
**École :** Grenoble INP – ESISAR  
**Date :** Mai 2026  
**Projet GitHub :** [Dog-Bot](https://github.com/H-BELIVEAU/Dog-Bot) (en cours de construction)

---

# Résumé

Ce projet consiste à concevoir et réaliser un robot quadrupède télécommandé capable d’exécuter plusieurs positions fixes et animations simples. Le système repose sur une architecture distribuée composée d’un ordinateur exécutant une interface Python, d’un Arduino UNO servant de passerelle de communication radiofréquence, et d’un ESP32 embarqué sur le robot pour le pilotage des servomoteurs via un contrôleur PWM PCA9685.

L’objectif principal du projet était de développer une plateforme robotique réutilisable et ouvert à de futures améliorations, tout en respectant des contraintes de coût et de temps.

Le robot utilise douze servomoteurs MG996R/MG946R, une communication RF bidirectionnelle par modules NRF24L01, une alimentation autonome par batterie et UBEC, ainsi qu’une structure mécanique majoritairement imprimée en PLA.

Malgré plusieurs difficultés importantes rencontrées pendant le projet — notamment une panne d’imprimante 3D et des problèmes de prototypage électronique — le projet a permis de valider l’ensemble de l’architecture électronique et logicielle, la communication RF, ainsi qu’une patte robotique entièrement fonctionnelle intégrant une cinématique directe et inverse opérationnelle.


---

# 1. Cahier des charges

## 1.1 Objectif général

L’objectif du projet est de concevoir un robot quadrupède piloté à distance capable d’exécuter plusieurs positions fixes et animations simples.

Le système doit être suffisamment modulaire et évolutif afin de permettre ultérieurement l’ajout de capteurs, d’algorithmes de stabilisation ou encore d’un mode de déplacement autonome.

---

## 1.2 Fonctionnalités attendues

Les principales fonctionnalités prévues pour le robot sont les suivantes :

- Pilotage à distance depuis un ordinateur
- Communication radiofréquence bidirectionnelle
- Contrôle individuel des servomoteurs
- Positions fixes : debout, assis, couché, lever une patte
- Animations simples : marche, rotation, petite danse
- Visualisation logicielle de la position des pattes
- Architecture extensible pour futurs capteurs et algorithmes autonomes

---

## 1.3 Contraintes du projet

Le projet devait respecter plusieurs contraintes techniques et pédagogiques.

### Contraintes matérielles

- Utilisation obligatoire d’un microcontrôleur simple (Arduino ou ESP32)
- Alimentation autonome et transportable
- Structure mécanique majoritairement imprimée en PLA
- Communication sans fil fiable

### Contraintes économiques

Le budget total du projet devait rester proche d’une centaine d’euros.

### Contraintes temporelles

Le projet devait être réalisé sur une durée limitée six semaines avec cinq séances encadrées.

---

## 1.4 Choix d’architecture globale

Afin de répondre aux objectifs du projet, une architecture distribuée a été retenue.

Le système est composé de trois blocs principaux :
- Un PC exécutant l’interface utilisateur
- Un Arduino UNO servant de passerelle RF
- Un ESP32 embarqué sur le robot

J'ai choisis de répartir le système de cette façon, pour pouvoir tout d'abord le réutiliser sur d'autres projets de commandes à distance. J'aime vraiment bien, car elle me permet de relier une interface graphique facile à coder avec Python, à un système électronique embarqué.

Ainsi l’ESP32 est chargé des tâches temps réel liées au pilotage des servomoteurs, tandis que les calculs et animations sont en réalité générés côté PC.

---

# 2. Architecture générale du système

## 2.1 Vue d’ensemble

Le fonctionnement global du système repose sur une chaîne de traitement répartie entre plusieurs composants.

L’utilisateur interagit avec une interface graphique développée en Python sur ordinateur. Cette interface génère des commandes qui sont envoyées via USB à un Arduino UNO.

L’Arduino UNO agit comme une passerelle, qui convertit ces commandes en paquets radiofréquence transmis à un ESP32 embarqué sur le robot à l’aide de modules NRF24L01.

L’ESP32 reçoit ensuite les données, interprète les commandes et pilote les servomoteurs via un contrôleur PWM PCA9685 connecté en I2C, et renvoie des données vers l'Arduino UNO.

---

## 2.2 Flux de données

Le système utilise plusieurs protocoles de communication.

![[Pasted image 20260528233032.png]]
### Communication PC <-> Arduino UNO <-> ESP32

La liaison entre le PC et l’Arduino UNO est réalisée en USB via une communication série UART et la communication sans fil utilise des modules NRF24L01 fonctionnant en 2.4 GHz.


![[Pasted image 20260528233521.png]]
### Communication ESP32 -> PCA9685 -> Servomoteurs
Le contrôleur PWM est relié à l’ESP32 via un bus I2C.
Les servomoteurs sont pilotés par signaux PWM générés par le PCA9685.

---

# 3. Conception matérielle

## 3.1 Microcontrôleurs

### Arduino UNO

L’Arduino UNO joue le rôle d’interface entre le PC et le robot.

Il est connecté au PC par USB et communique avec le module NRF24L01 via le protocole SPI. Son rôle principal consiste à convertir les données reçues du PC en paquets exploitables par le système radiofréquence.

L’utilisation d’un Arduino UNO permet également de rendre l’architecture réutilisable pour d’autres projets nécessitant une communication RF simple entre un ordinateur et un microcontrôleur distant.

### ESP32

L’ESP32 constitue le cœur du robot.

Ce microcontrôleur a été choisi pour plusieurs raisons :
- puissance de calcul supérieure à un Arduino UNO
- compatibilité avec l’environnement Arduino
- présence d’un régulateur 3.3V
- gestion native du Wifi et du Bluetooth (pour améliorations futures)
- grand nombre d’entrées/sorties.

L’ESP32 est responsable du pilotage des servomoteurs et de la communication avec le contrôleur PWM PCA9685.

---

## 3.2 Communication radiofréquence

La communication sans fil repose sur deux modules NRF24L01 PA LNA équipés d’antennes externes.

Ces modules fonctionnent sur la bande de fréquence 2.4 GHz et utilisent le protocole SPI pour communiquer avec les microcontrôleurs.

Un condensateur de 47 µF a été ajouté à proximité de chaque module afin de stabiliser l’alimentation et réduire les perturbations liées aux pics de courant.

Le système utilise un échange bidirectionnel alternant rapidement les phases d’émission et de réception.

Cette architecture permet :
- l’envoi des commandes de contrôle ;
- la réception d’accusés de réception ;
- l’ajout futur de retours capteurs.

Cependant, une seule antenne ne peux jamais envoyer et recevoir des données en même temps. Alors pour avoir une communication bidirectionnelle, on créé un tunnel dans chaque sens de communication, lorsque l'on veut envoyer un message, on passe en mode d'écriture, mais tout le reste du temps, on reste en mode réception.

---

## 3.3 Contrôleur PWM PCA9685

Le PCA9685 est un contrôleur PWM 16 canaux utilisant le protocole I2C.

Son rôle est de générer les signaux PWM nécessaires au pilotage des servomoteurs.

L’utilisation de ce composant présente plusieurs avantages :
- génération matérielle des PWM
- pilotage simultané de nombreux servomoteurs
- alimentation séparée pour les actionneurs

Le PCA9685 possède une résolution de 12 bits, soit 4096 niveaux possibles (de signaux entre 1ms et 2ms).

---

## 3.4 Actionneurs

Le robot utilise principalement des servomoteurs MG996R ainsi que quelques MG946R pour les articulations les plus sollicitées.

Chaque patte possède trois degrés de liberté :
- épaule
- coude
- cheville

Les MG946R se chargent seuls de l'axe des épaules, et le coude et la cheville sont formés par deux MG996R.

Les servomoteurs ont été choisis pour leur faible coût et leur couple relativement élevé.

Cependant, leur consommation importante impose une alimentation capable de fournir plusieurs ampères simultanément (plus de 1.5A en charge, donc plus de 20A au total).

---

## 3.5 Alimentation

Le système d’alimentation repose sur deux branches distinctes :
- une branche logique 3.3V (depuis l'ESP32)
- une branche puissance 5V

Une batterie Lipo devait initialement être utilisée, mais un important retard de livraison, au delà de la date limite du projet, a conduit à utiliser temporairement une batterie de trottinette électrique récupérée, qui est compatible, mais beaucoup trop grande et lourde.

La tension de la batterie est régulée à l’aide d’un UBEC 5V haute puissance capable de fournir jusqu’à 30A.

Toutes les masses du système sont reliées afin d’assurer une référence électrique commune.

---

## 3.6 Circuit imprimé

Après plusieurs essais sur plaque de prototypage, un véritable circuit imprimé a été conçu sous KiCad, car j'ai eu trop de problèmes, entre des faux contacts et des fils qui ne tenaient simplement pas.

Le PCB m'a permis de fiabiliser les connexions et de simplifier le remplacement des modules, car les composants ne sont pas directement soudés sur le PCB mais montés sur connecteurs afin de pouvoir être remplacés facilement.

Le routage a été réalisé avec plusieurs précautions :
- utilisation d’un plan de masse
- pistes d’alimentation larges
- orientation des antennes vers l’extérieur
- éloignement partiel des zones RF

Le PCB a fonctionné correctement dès les premiers tests, à l'exception d'un fil qui avait été oublié, mais rapidement corrigé.

Voici les schémas électriques utilisés :
![[ElectricSchematic.png]]
-> Schéma électrique du robot

![[CuivreB.png|197]]
![[CuivreF.png|196]]
-> Couches de cuivres du PCB

![[Pasted image 20260529075720.png]]
![[Pasted image 20260529075703.png|506]]
-> Schéma électrique de la passerelle

---

## 3.7 Conception mécanique

La structure du robot est majoritairement imprimée en PLA.

![[TazerEngineering.png|697]]
Le mécanisme des pattes s’inspire du travail du Youtubeur TazerEngineering, mais a été entièrement redessiné pour être adapté aux contraintes du projet, notamment le fait qu'elle sera entièrement imprimée en PLA.

Chaque patte est entièrement démontable afin de faciliter le remplacement des servomoteur et le transport, et les servomoteurs sont regroupés près du corps du robot afin de limiter les masses en mouvement, ce qui permet de réduire les contraintes mécaniques sur les articulations.

---

# 4. Architecture logicielle

## 4.1 Interface PC

L’interface utilisateur a été développée en Python, avec les bibliothèques pygame, PyQT6.
Le logiciel permet le contrôle manuel d'une patte du robot, la génération d'animations simples, le tout avec visualisation de la cinématique, et gestion d'une manette.
Il y a aussi un programme de test utilisant tkinter, pour pouvoir facilement rentrer dans un tableau les valeurs d'angles des servomoteurs.

L’interface graphique repose sur la bibliothèque PyQt6.
Cette bibliothèque permet de construire une interface événementielle complète avec boutons, affichages et visualisations, et de relier des événements spécifiques (clic sur un bouton, donnée reçue) à des fonctions, le tout de manière asynchrone.
La communication série avec l’Arduino UNO utilise la bibliothèque pyserial et la gestion de la manette de contrôle est réalisée avec pygame.

---

## 4.2 Protocole de communication

Un protocole de communication simple et réutilisable a été conçu pour l’échange de données.

Les messages suivaient initialement le format : KEY:TYPE:DATA, mais ont été remplacées par seulement :::KEY:DATA, pour pouvoir envoyer plus de données par paquets (les paquets RF sont limités à 32o).
Les données envoyées sur le Serial commençant par ":::" sont directement transmises à l'ESP32, tandis que celles commençant par "CMD:XXX" effectuent une commande par défaut (par exemple, CMD:PING effectue un PING vers le robot pour vérifier la connexion).

Pour contrôler spécifiquement un servomoteur, on utilise : ":::Servo_ABC:X", avec A: B ou F pour Back ou Front, B: L ou R pour Left ou Right, C: S, A ou E pour Shoulder, Ankle ou Elbow, et enfin X la valeur d'angle entière en degrés.
Pour envoyer les données de tous les servomoteurs, on utilise : ":::Servo_ALL:ABCDEFGHIJKL", avec pour chaque lettre la valeur d'un servomoteur, et chaque lettre séparées par "\0".
On a encore la commande ":::RELEASE:ALL" pour relâcher tous les servomoteurs, ou ":::RELEASE:ABC", avec A, B, et C comme juste avant pour relâcher qu'un seul servomoteur, et finalement on a la commande ":::POSITION:X", avec X la position prédéfinie à faire.


---

## 4.3 Arduino UNO

Le programme de l’Arduino UNO joue uniquement un rôle de passerelle.
- lit les données reçues via Serial
- convertit les messages en paquets RF
- transmet les données
- reçoit les réponses
- renvoie les informations vers le PC

Aucune logique de contrôle robotique n’est exécutée sur cette partie.

---

## 4.4 ESP32

L’ESP32 reçoit les commandes radiofréquence et pilote directement les servomoteurs.
Le microcontrôleur utilise la bibliothèque Adafruit PWM Servo Driver pour communiquer avec le PCA9685.
Chaque instruction reçue correspond actuellement à une commande de position d’un servomoteur. 
Le système convertit les angles en valeurs PWM adaptées avant transmission au PCA9685.

Pour éviter les pics de courant, lorsque l'on bouge trop de servos en même temps (problème qui a très sûrement provoqué la panne du servomoteur de l'épaule cassée), j'ai adapté le code, pour que chaque servomoteur ne se déplace que petit à petit, en stockant pour chaque servomoteur son angle actuel, et son angle à atteindre, et j'ai également rajouté un mini temps de pause entre deux déplacements de servomoteurs. 
Par manque de temps, j'ai préféré à partir de ce moment là de ne pas continuer les objectifs d'animations du chien, pour éviter de casser plus de matériel.

---

# 5. Modélisation et cinématique


L’objectif principal est de pouvoir contrôler précisément la position du pied d’une patte dans l’espace afin de générer des animations simplement en suivant un tracés de points.

Pour cela, il est nécessaire de modéliser mathématiquement le mécanisme de la patte, puis de développer des outils de cinématique directe et inverse.

La cinématique directe permet de déterminer la position du pied à partir des angles des moteurs, tandis que la cinématique inverse réalise l’opération opposée : calculer les angles nécessaires pour atteindre une position donnée.
![[IMG_5439.png]]

---

## 5.1 Modélisation géométrique

Afin de simplifier les calculs, la patte du robot a été modélisée comme un système plan articulé.

Le mécanisme principal est constitué de deux servomoteurs travaillant dans un même plan, et formant à deux, un coude et une cheville. Le troisième servomoteur, placé au niveau de l’épaule, permet ensuite de mettre ce plan en rotation afin de déplacer la patte latéralement.

Pour établir les équations du système, un repère orthonormé a été défini à partir de l’axe principal de la patte.

Le point correspondant au premier servomoteur est choisi comme origine du repère.

Les différentes longueurs des liaisons mécaniques ont été déterminées lors de la conception mécanique sous Fusion 360, puis ajustées progressivement à l’aide des simulations et des essais physiques.

Le système mécanique utilise plusieurs parallélogrammes articulés afin de conserver certaines orientations pendant le mouvement, de mieux répartir les efforts mécaniques, de limiter les contraintes sur les servomoteurs et finalement de réduire les masses en mouvement.

La modélisation mathématique a été développée en parallèle de la modélisation 3D afin de vérifier en permanence la cohérence entre les contraintes géométriques et les amplitudes des servomoteurs.

Le modèle géométrique obtenu sert ensuite de base aux calculs de cinématique directe et inverse.

On obtient notamment la position du bout de la patte en fonction des paramètres et des constantes :
$M(x,y) = (L2.cos(\Theta1)-\frac{L1}{L2}.cos(\Theta2+\alpha),\ L2.sin(\Theta1)-\frac{L1}{L2}.sin(\Theta2+\alpha))$

---

## 5.2 Cinématique directe

La cinématique directe consiste à déterminer la position du pied à partir des angles des servomoteurs.

Dans le cas du robot, les angles des servomoteurs sont connus, et l’objectif est de calculer la position du point représentant le pied de la patte.

Le système est modélisé à l’aide de plusieurs points et vecteurs définis dans le plan.

Les paramètres sont :
- $L1$ pour les premières liaisons ;
- $L2$ pour les segments secondaires.
- $\alpha$ pour l'angle formé par le triangle

Les angles des servomoteurs sont notés $\Theta_1$ et $\Theta_2$.

Les positions intermédiaires des différents points du mécanisme peuvent alors être déterminées par trigonométrie simple.

Ces calculs sont utilisés directement dans l’interface graphique afin de visualiser en temps réel la position de la patte lorsque les servomoteurs sont déplacés, ou pour vérifier en simulation les animations.

---

## 5.3 Cinématique inverse

La cinématique inverse réalise l’opération opposée à la cinématique directe.

L’objectif est ici de déterminer les angles des servomoteurs nécessaires pour atteindre une position précise du pied dans l’espace.

Cette partie constitue l’un des éléments les plus importants du projet, car elle permet de transformer des trajectoires géométriques en mouvements physiques exploitables par le robot.

Concrètement, lorsqu’une position cible $(x,y)$ est choisie dans l’interface graphique, le programme doit calculer automatiquement les angles à envoyer aux servomoteurs afin que la patte atteigne cette position.

Le calcul utilise la loi des cosinus en partant des résultats trouvés par la modélisation directe, et les propriétés des parallélogrammes.

On obtient alors les angles des servomoteurs en fonction d'une position $(x,\ y)$ :
(on note $r = \sqrt{x^2+y^2}$)
$\Theta1 = arctan(\frac{y}{x}) - arccos(\frac{L1^2+r^2-4.L2^2}{2.L1.r})$
$\Theta2 = arctan(\frac{y}{x}) - arccos(\frac{4L2^2+r^2-L1^2}{4.L2.r})$

Le programme vérifie également que les angles calculés restent dans les limites physiques des servomoteurs, et dessine aussi une zone atteignable sur une simulation.

Vérifications : Pour qu'une solution existe, il faut que :
1) $r$ soit compris entre $L1-2.L2$ et $L1+L3$ (deux cercles formés autours des servomoteurs)
2) les valeurs de arccos soient définies
3) les angles se trouvent entre les bornes définies des servomoteurs

Les premiers tests physiques ont montré un fonctionnement correct du modèle dès les premiers essais sur prototype simplifié, cependant, elle n'a pas été terminée, car elle ne prédit que les mouvements sur le plan, sans prendre en compte l'épaule.

---

## 5.4 Génération des animations

Les animations du robot sont générées à partir de trajectoires définies dans l’espace cartésien.

Plutôt que de contrôler directement les angles des servomoteurs, le système définit d’abord la trajectoire souhaitée du pied, puis utilise la cinématique inverse pour convertir cette trajectoire en commandes moteurs.

Cette méthode permet d’obtenir des mouvements plus naturels et plus simples à concevoir.

Le principe général d’une animation est le suivant :
1. définition d’une trajectoire avec des points
2. découpage temporel en plusieurs positions intermédiaires (interpolation pour être plus fluide)
3. calcul des angles associés
4. transmission des commandes au robot.

Les animations ont fonctionnées sur la patte de prototype, avec une animation de position fixe, et une animation de marche.

D'ailleurs, le cycle de marche repose sur deux phases principales :
- une phase d’appui au sol (ligne droite)
- une phase de retour aérienne (demi cercle)

Pendant la phase d’appui, le pied reste proche du sol afin de pousser le robot vers l’avant, pendant la phase de retour, la patte est relevée afin d’éviter les obstacles avant de revenir à sa position initiale, et il y a plus de temps sur la phase d'appui que sur la phase de retour, pour avoir un maximum de temps d'appuie.

Par manque de temps et afin d’éviter d’endommager davantage les servomoteurs, les animations complexes n’ont pas été entièrement finalisées sur le robot entier.

---

# 6. Développement du projet

## 6.1 Phase de réflexion et premières recherches

Le projet a commencé par une importante phase de recherche et de réflexion autour des différentes architectures possibles pour un robot quadrupède.

Plusieurs problématiques devaient être étudiées dès le départ :
- architecture mécanique des pattes ;
- choix des actionneurs
- système d’alimentation
- architecture électronique
- protocole de communication
- faisabilité mécanique

Le projet de TazerEngineering a notamment servi d’inspiration pour une partie de la conception mécanique des pattes.

Cependant, la majorité des pièces ont ensuite été redessinées et adaptées aux contraintes du projet, notamment le fait que ma patte devait être entièrement imprimée en PLA.

---

## 6.2 Conception mécanique

La conception mécanique a été réalisée entièrement sous Fusion 360.

J'ai eu plusieurs versions avant de trouver la bonne façon de fixer mes pièces à mes servomoteurs, les pièces ont donc été progressivement modifiées et améliorées au fil des essais, et je suis arrivé à la conclusion que le plus solide était de visser mes pièces à un adaptateur fournit avec le servomoteur, car le PLA ne supportait pas la charge, et finissait par ne plus accrocher l'axe du servomoteur.

J'ai d'ailleurs choisis de tout imprimer en PLA, car il est le plus simple à travailler et rapide à imprimer, même s'il supporte moins la charge ou la chaleur.

![[Pasted image 20260529084313.png]]
![[IMG_5440.png]]

![[Pasted image 20260529092548.png]]
![[IMG_5441.png]]

---

## 6.3 Développement électronique

Le développement électronique a commencé par la réalisation de plusieurs prototypes sur breadboard puis sur plaques de prototypage.

L’objectif initial était de valider progressivement un circuit fonctionnel, puis de tout souder pour ne plus avoir de problèmes de mauvaises connexions, notamment à cause des déplacements.

Les premiers essais ont rapidement mis en évidence plusieurs difficultés liées au prototypage, avec des faux contacts, une alimentation instable, des difficultés de soudures et un manque de fiabilité mécanique.

Le modules NRF24L01 s'est montré particulièrement sensible à la qualité de l’alimentation électrique.  L’ajout de condensateurs électrolytiques de 47µF à proximité des modules RF a permis d’améliorer fortement la stabilité du système.

Après plusieurs essais, il est rapidement devenu évident qu’un véritable circuit imprimé serait nécessaire afin de fiabiliser l’ensemble.

La conception du PCB a été réalisée sous KiCad.

Le circuit imprimé intègre :
- ESP32
- NRF24L01
- PCA9685
- connecteurs d’alimentation
- condensateur de découplage

Une attention particulière a été portée :
- aux largeurs de pistes d’alimentation
- à la disposition des composants RF
- au plan de masse
- à la modularité du système.

Afin de simplifier les réparations et les remplacements de composants, les principaux modules ont été montés sur connecteurs plutôt que soudés directement.

Les premiers tests du PCB ont été très encourageants.  
Contrairement aux prototypes précédents, le système s’est révélé immédiatement beaucoup plus stable et fiable.

Cette étape a marqué une amélioration majeure dans le projet.

Le seul problème électrique est survenu bien plus tard, lorsque tous les servomoteurs démarraient simultanément, l'alimentation ne réussissait pas à tous les déplacer, et un des servomoteurs (épaule arrière gauche) s'est cassé.

---

## 6.7 Intégration finale

La phase finale du projet consistait à assembler l’ensemble des sous-systèmes :
- circuit imprimé
- systèmes des pattes
- alimentation
- planche 

L’objectif était d’obtenir un robot quadrupède entièrement fonctionnel capable de réaliser plusieurs animations simples.

Cependant, plusieurs difficultés matérielles ont fortement limité cette phase :
- panne d’imprimante 3D
- casse d’un servomoteur
- temps restant très limité

Face à ces contraintes, j'ai décidé de privilégier la protection du matériel restant, en n'utilisant pas pour l'instant toutes les pattes du robot.

Même si le robot complet n’a pas pu être finalisé avant la fin du projet, l’ensemble des briques technologiques principales ont pu être validées individuellement, avec :
- communication RF bidirectionelle
- contrôle de servomoteurs
- cinématique (directe et inverse)
- circuit imprimé
- interface logicielle
- alimentation
- architecture distribuée

Le projet a donc permis d’obtenir une base robotique solide et évolutive pouvant être poursuivie ultérieurement.

---

# 7. Difficultés rencontrées

## 7.1 Difficultés de prototypage

Les premières phases de prototypage électronique ont été particulièrement compliquées.

Les essais réalisés sur plaques de prototypage présentaient de nombreux problèmes :
- faux contacts
- câblage fragile
- erreurs difficiles à identifier (des câbles qui prennent parfois de la tension)
- alimentation instable

Les problèmes de soudure ont également fortement compliqué les premiers tests.

Certaines connexions devenaient intermittentes selon les mouvements du prototype, rendant le débogage particulièrement difficile.

La réalisation du PCB a permis de résoudre une grande partie de ces difficultés.

## 7.2 Difficultés liées aux communications RF

Les communications radiofréquence ont également posé plusieurs problèmes pendant le développement.

Les modules NRF24L01 sont relativement sensibles aux perturbations électriques, ce qui mène à des pertes de paquets ou des redémarrages aléatoires.
Ces problèmes étaient principalement liés aux pics de consommation générés lorsque plusieurs servomoteurs démarraient simultanément.

Le système d’alimentation initial n’était pas suffisamment stable pour absorber ces variations de courant.

Seulement l'ajout d'un condensateur de découplage de 47µF m'a permis de résoudre ce problème.

Un des modules NRF24L01 a également été endommagé pendant les essais, probablement à cause d’un court-circuit ou d’une alimentation instable (avec le PCB de prototypage).

Malgré ces difficultés, les communications sont finalement devenues suffisamment fiables pour les besoins du projet, avec en moyenne un temps d'aller retour moyen de 4ms sur les tests réalisés.

## 7.3 Difficultés liées à l’alimentation

L’alimentation électrique a constitué l’un des aspects les plus complexes du projet.

Les servomoteurs MG996R consomment des courants importants, particulièrement lors des démarrages, lors des changements brusques de direction ou encore lorsqu’ils subissent une forte contrainte mécanique.

Au début du projet, ces consommations avaient été sous-estimées.

Plusieurs problèmes sont rapidement apparus, avec des chutes de tension (qui mènent à un redémarrage des microcontrôleurs), des échauffements, et un comportement imprévisible des servos.

La batterie Lipo initialement prévue pour le projet a également subi un important retard de livraison, mais une batterie de trottinette électrique récupérée était compatible.
Même si cette solution était peu pratique à cause de son poids et de son encombrement, elle a permis de continuer les essais sans interrompre complètement le projet.

Je pense que la gestion de l'alimentation pourrait être améliorée, surtout en utilisant la prochaine fois un meilleur multiplexeur de canaux PWM, car je pense qu'il n'était pas capable de faire passer assez de courant en pic, ce qui a du jouer dans la casse du servomoteur.

Egalement, lorsque les servomoteurs créent un pic de courant, la tension du système chute, donc l'ESP32 est forcé à redémarrer. Une correction à faire dans le futur serait d'ajouter un gros condensateur pour compenser la baisse de tension.

## 7.4 Difficultés mécaniques

La mécanique du robot a nécessité de nombreuses essais.

Plusieurs problèmes sont apparus au cours des premiers assemblages, avec surtout des pièces trop fragiles, des limitations d'angles (autour des liaisons pivot).

Le choix du PLA, bien que très pratique pour le prototypage rapide, présente certaines limites mécaniques, car il est assez fragile, se déforme facilement et ne résiste pas aux efforts répétés.

Les servomoteurs eux-mêmes imposaient également plusieurs contraintes :
- amplitudes limitées ;
- couples variables ;
- précision imparfaite ;
- présence de jeux internes.

L’intégration mécanique complète du robot s’est révélée beaucoup plus complexe que prévu initialement, surtout avec le manque de temps, à cause des deux semaines de pannes de l'imprimante 3D.

## 7.5 Panne de l’imprimante 3D

La panne de l’imprimante 3D a constitué la difficulté majeure du projet.

L’imprimante étant utilisée pour produire pratiquement toutes les pièces mécaniques, cette panne a eu un impact direct sur tout le reste du projet.
Plusieurs composants de l’imprimante ont dû être remplacés, entraînant plusieurs semaines d’arrêt.

Pendant cette période, il m'était impossible d'imprimer de nouvelles pièces, de les tester, et de les corriger. Alors j'ai accumulé à ce moment énormément de retard.

Une grande partie du temps de fin de projet a donc été consacrée à rattraper ce retard.

## 7.6 Casse du servomoteur d’épaule

Lors des derniers essais mécaniques, un servomoteur situé au niveau de l’épaule a été endommagé.

Je ne suis pas sûr de l'origine de la panne, mais elle doit sûrement venir d'un pic de courant trop brutal.

Afin d’éviter d’endommager davantage le matériel, plusieurs protections logicielles ont été ajoutées : limitation de vitesse, avec un déplacement progressif, des temporisations, et une réduction du nombre de mouvements simultanés.

À partir de ce moment, il a été décidé de privilégier la validation des sous-systèmes, la stabilité de l'architecture, mais surtout de la protection du matériel restant.

Cette décision a conduit à réduire fortement les objectifs d’animations avancées prévus initialement et de ne pas tenter de faire marcher le robot.

# 8. Résultats obtenus

## 8.1 Validation de l’architecture globale

Malgré les différentes difficultés rencontrées pendant le projet, l’architecture générale du robot me semble validée avec succès.

L’ensemble de la chaîne de fonctionnement a été testé : interface PC communiquant en série avec la passerelle transmettant en RF, et contrôle du PCA9685 par l'ESP32 pour piloter des servomoteurs, le tout avec des protocoles de contrôles très évolutif (architecture d'envoie de commandes à distance personnalisables).

Cette organisation constitue désormais une base solide pour les futures évolutions du robot.

---

## 8.2 Validation des communications RF

Les communications radiofréquence entre l’Arduino UNO et l’ESP32 ont été validées avec succès.

Le système permet :
- l’envoi de commandes individuelles
- l’envoi de positions globales
- la réception de réponses
- la vérification de la connexion

Les tests ont montré une communication relativement stable à courte distance malgré les perturbations générées par les servomoteurs.

Les échanges sont suffisamment rapides pour permettre un contrôle temps réel des servomoteurs.

---

## 8.3 Validation du contrôle des servomoteurs

Le contrôle des servomoteurs via le PCA9685 a été presque entièrement validé, à l'exception des problèmes d'alimentation.

Chaque servomoteur peut être :
- contrôlé individuellement
- relâché indépendamment des autres

Le système de limitation de vitesse ajouté en fin de projet a permis de réduire les pics de courants, et donc de réduire les risques de casse. J'ai aussi remarqué que les servomoteurs MG946R des coudes chauffaient énormément, même relâchés, j'ai donc décidé de ne plus les utiliser.

---

## 8.4 Validation de la cinématique

Les modèles de cinématique directe et inverse développés pendant le projet ont été validés expérimentalement.

Les positions calculées correspondent correctement aux mouvements physiques observés sur les prototypes.

La cinématique inverse permet de contrôler précisément la position du pied pour générer des trajectoires et donc simplifier la création d’animations.

La visualisation en temps (qui elle utilise la cinématique directe) réel intégrée dans l’interface graphique a fortement facilité cette validation.

Les essais réalisés sur prototype simplifié ont montré que les équations sont cohérentes et que les angles calculés sont exploitables physiquement.

---

## 8.5 Validation du PCB

Le circuit imprimé conçu sous KiCad a fonctionné correctement dès les premiers essais, à l'exception d'un fil qui a été rajouté.

Le PCB a permis d’améliorer fortement :
- la stabilité électrique
- la fiabilité des connexions
- la qualité du câblage
- l’intégration globale

Contrairement aux prototypes sur plaques de prototypage, le système est devenu beaucoup plus stable après l’intégration du PCB.

Les alimentations sont mieux réparties et les perturbations RF ont été fortement réduites.

Le seul problème trouvé au circuit actuel, est qu'il n'a pas été prévu assez évolutif. Il n'y a par exemple pas de connecteurs prévus en plus, pour un futur ajout de capteurs.

---

## 8.6 Validation de l’interface logicielle

L’interface développée en Python est assez fonctionnelle, bien que non terminée. En effet l'une des conséquences du manque de temps, m'a obligé à focaliser toute mon attention sur le code de l'ESP32, et le code de la passerelle Arduino. J'ai donc préféré faire le strict minimum avec plusieurs programmes courts, et pas une grande application qui gère tout.

Je peux cependant faire une simulation en même temps que de contrôler le robot, lancer des animations simples (position debout et marche d'une seule patte), mais aussi contrôler tous les servomoteurs du robot, en modifiant les valeurs des angles directement sur un tableau.

---

# 9. Conclusion

Le robot complet n’a malheureusement pas pu être entièrement finalisé avant la fin du projet.

Plusieurs facteurs ont fortement limité l’intégration finale :
- panne prolongée de l’imprimante 3D
- casse d’un servomoteur
- temps restant limité
- risques élevés pour le matériel restant

Cependant, l’ensemble des sous-systèmes principaux ont été validés séparément :
- électronique
- communication
- logiciel
- alimentation
- cinématique
- pilotage des servomoteurs

Le projet forme une architecture très complète, stable et évolutive d'un robot, qui pourra être une base solide pour de futurs développements, et m'a permis d'identifier un grand nombre de problèmes que je pourrai corriger sur mes prochains projets.
