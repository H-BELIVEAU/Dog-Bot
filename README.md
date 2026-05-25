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
│   │   └── PC Python interface
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
    ├── fusion/
    │   └── (.f3d) Fusion 360 files
    │
    └── printable/
        ├── (.stl) 3D printable files
        └── (+Kobra S1 files)

```

## Documentation
Documentation folders are Obsidian projects.
- [French documentation](docs/fr/Projet%20Chien%20Robot.md)
- [English documentation](docs/en/Dog%20Bog%20Project.md)
