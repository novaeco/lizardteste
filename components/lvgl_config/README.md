# Configuration LVGL pour NovaReptileElevage

Ce composant ne contient **pas** les sources de LVGL. Il publie uniquement la
configuration `lv_conf.h` utilisée par le projet et laisse l'ESP-IDF Component
Manager télécharger la bibliothèque officielle `lvgl/lvgl` en version 9.4.x.

## Utilisation

1. Vérifiez que vous travaillez avec ESP-IDF ≥ 6.1 et que le gestionnaire de
   composants est connecté à internet lors du premier build.
2. Depuis la racine du projet, exécutez (une seule fois) :
   ```bash
   idf.py add-dependency "lvgl/lvgl^9.4.0"
   ```
   Le manifeste `idf_component.yml` du composant `main` référence également cette
   dépendance pour automatiser le téléchargement sur les CI.
3. Construisez le firmware normalement :
   ```bash
   idf.py build
   ```

## Contenu

- `CMakeLists.txt`
  - Exporte `components/lvgl_config` dans les include globaux du projet afin que
    `#include "lv_conf.h"` fonctionne pour toutes les cibles, y compris le
    composant LVGL téléchargé via le Component Manager.
- `lv_conf.h`
  - Paramétrage LVGL adapté à l'écran Waveshare ESP32-S3 Touch LCD 7B
    (1024×600), activation des modules requis et allocation PSRAM (128KB).
- `lvgl.h`
  - Wrapper d'inclusion simplifié utilisé par les sources locales.

## Notes

- Aucun fichier source LVGL n'est versionné pour éviter un dépôt massif (>20k
  fichiers).
- Lors d'une compilation hors-ligne, assurez-vous d'avoir préchargé la
  dépendance (`idf.py add-dependency`), sinon CMake échouera en recherchant le
  composant `lvgl`.
