# Intégration LVGL (configuration projet)

Ce composant fournit uniquement la configuration `lv_conf.h` et la redirection d'en-têtes nécessaire pour exploiter LVGL 9.4.x livré par le Component Manager d'ESP-IDF.

## Dépendance LVGL via Component Manager

Le projet ne versionne plus les sources LVGL. La bibliothèque est récupérée automatiquement depuis le registre officiel :

```bash
idf.py add-dependency "lvgl/lvgl^9.4.0"
```

Le manifeste `main/idf_component.yml` déclare déjà cette dépendance ; lancer `idf.py build` téléchargera LVGL si elle n'est pas encore présente dans le cache global de l'ESP-IDF.

## Rôle du composant `lvgl_config`

- expose `lv_conf.h` et `lvgl.h` sur l'include path global ;
- force la définition `LV_CONF_INCLUDE_SIMPLE=1` sur la cible LVGL officielle ;
- évite toute duplication de la bibliothèque tout en conservant une configuration projet centralisée.

## Contenu

```
components/lvgl_config/
├── CMakeLists.txt   # Déclare la dépendance LVGL et applique LV_CONF_INCLUDE_SIMPLE
├── README.md        # Ce fichier
├── lv_conf.h        # Configuration LVGL personnalisée du projet
└── lvgl.h           # Wrapper vers <lvgl/lvgl.h>
```

## Mise à jour de la configuration

Ajustez les options graphiques (polices, widgets, drivers…) directement dans `lv_conf.h`. Toute modification sera immédiatement propagée aux builds utilisant LVGL 9.4.x du Component Manager.
