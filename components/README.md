# Composants ESP-IDF pour NovaReptileElevage

Ce dossier regroupe les composants personnalisés qui complètent ceux distribués via l'ESP-IDF Component Manager.

## Composants inclus dans le dépôt

### LVGL Config
- **Version LVGL ciblée** : 9.4.x (téléchargée automatiquement)
- **Usage** : expose `lv_conf.h` et le wrapper `lvgl.h`, applique `LV_CONF_INCLUDE_SIMPLE` sur la bibliothèque externe.
- **Localisation** : `components/lvgl_config/`

### ST7701 RGB Panel Driver
- **Version** : interne (timings Waveshare 7B 1024×600)
- **Usage** : initialisation du contrôleur LCD via `esp_lcd` et configuration du pipeline RGB.
- **Localisation** : `components/st7701_rgb/`

### Pilotes CH422G & I2C utilitaires
- **Usage** : expander GPIO pour rétroéclairage et bus I²C partagé (GT911, CH422G, etc.).
- **Localisation** : `components/ch422g/`, `components/i2c_bus/`

## Dépendances récupérées automatiquement

Le fichier `main/idf_component.yml` déclare l'utilisation de LVGL 9.4.x :

```bash
idf.py add-dependency "lvgl/lvgl^9.4.0"
```

Lors d'un `idf.py build`, ESP-IDF télécharge la bibliothèque dans son cache global (`~/.espressif/components/`). Aucun fichier LVGL volumineux n'est versionné dans ce dépôt.

## Configuration LVGL

- Le fichier de configuration se trouve dans `components/lvgl_config/lv_conf.h`.
- Grâce à `LV_CONF_INCLUDE_SIMPLE`, tous les modules LVGL et l'application peuvent accéder à cette configuration.
- Les paramètres sont optimisés pour ESP32-S3 + écran 1024×600 avec contrôleur tactile GT911.
