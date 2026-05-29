# Dog Robot

-- Projet de première année d'élec à l'ESISAR --\
\
Réalisation d'un chien robot, contrôlable à distance par radio fréquence.

## Components list

* 1x - Arduino UNO
* 1x - ESP32
* 1x - PCA9685
* 2x - NRF24L01 + PA + LNA
* 8x - MG996R
* 4x - MG946R
* 1x - UBEC 5V 30A
* 1x - Lipo 3300mAh battery


## Architecture

```txt
Dog Bot/
├── codes/
│   ├── gateway/
│   │   └── Arduino UNO code
│   │
│   ├── interface/
│   │   └── PC Python interface and tests
│   │
│   └── robot/
│       └── ESP32 robot code
│
├─── docs/ 
│   └── Project documentations
│
├─── electronics/ 
│   └── Electronics schematics
│
└── mechanical/
    └── (.stl) 3D printable files

```

## Documentation
Documentation folders are Obsidian projects.
- [French documentation](docs/fr/Projet%20Chien%20Robot.md)
