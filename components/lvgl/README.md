# Composant LVGL pour ESP-IDF

Ce dossier doit contenir la bibliothèque LVGL 9.4.x.

## Installation

Pour installer LVGL dans ce composant :

1. Téléchargez LVGL 9.4.x depuis GitHub :
   ```bash
   cd components/lvgl
   git clone --recurse-submodules https://github.com/lvgl/lvgl.git
   cd lvgl
   git checkout release/v9.4
   ```

2. Ou utilisez le gestionnaire de composants ESP-IDF :
   ```bash
   cd components
   idf.py add-dependency "lvgl/lvgl^9.4.0"
   ```

## Configuration

Le fichier `lv_conf.h` se trouve dans `components/lvgl/lv_conf.h` et est configuré pour :
Grâce à la définition `LV_CONF_INCLUDE_SIMPLE`, ce fichier unique est accessible aux sources du composant et de l'application.
- Écran 1024x600 16-bit (RGB565)
- Tactile GT911
- Widgets essentiels activés
- Polices Montserrat

## Structure

```
components/lvgl/
├── CMakeLists.txt          # Configuration du composant
├── README.md              # Ce fichier
└── lvgl/                  # Sources LVGL (à télécharger)
    ├── src/
    ├── examples/
    └── lv_conf_template.h
```
