# Composants ESP-IDF pour NovaReptileElevage

Ce dossier contient les composants externes utilisés par le projet.

## Composants inclus

### LVGL (Light and Versatile Graphics Library)
- **Version** : 9.4.0
- **Usage** : Interface utilisateur graphique
- **Localisation** : `components/lvgl/`

### ST7701 RGB Panel Driver
- **Version** : interne (dérivé des timings Waveshare 7B)
- **Usage** : Initialisation du contrôleur LCD 1024x600 via `esp_lcd`
- **Localisation** : `components/st7701_rgb/`

## Installation des composants

### Méthode 1 : Git Submodules (Recommandée)
```bash
cd components/lvgl
git clone --recurse-submodules https://github.com/lvgl/lvgl.git
cd lvgl
git checkout release/v9.4
```

### Méthode 2 : ESP Component Manager
```bash
idf.py add-dependency "lvgl/lvgl^9.4.0"
```

## Vérification

Après installation, la structure doit être :
```
components/
├── README.md
└── lvgl/
    ├── CMakeLists.txt
    ├── README.md
    └── lvgl/           # Sources LVGL
        ├── src/
        ├── examples/
        └── lv_conf_template.h
```

## Configuration

- Le fichier de configuration LVGL se trouve dans `components/lvgl/lv_conf.h`
- Ce fichier est partagé dans tout le projet grâce à la macro `LV_CONF_INCLUDE_SIMPLE`
- Les paramètres sont optimisés pour ESP32-S3 avec écran 1024x600
- Support tactile GT911 activé
