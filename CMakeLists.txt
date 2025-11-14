cmake_minimum_required(VERSION 3.24)
set(PROJECT_NAME "nova_reptile_elevage")

# Configuration du projet NovaReptileElevage
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(${PROJECT_NAME})

idf_build_set_property(SDKCONFIG_DEFAULTS "sdkconfig.defaults")

if(NOT DEFINED ENV{IDF_TARGET})
    set(ENV{IDF_TARGET} esp32s3)
    message("IDF_TARGET not set, defaulting to: $ENV{IDF_TARGET}")
endif()


# Configuration spécifique LVGL
idf_build_set_property(COMPILE_DEFINITIONS "LV_CONF_INCLUDE_SIMPLE=1" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "LV_FONT_MONTSERRAT_30=1" APPEND)

# ===============================================
# EXPLICATION DÉTAILLÉE DU FICHIER CMAKE RACINE
# ===============================================

# 1. cmake_minimum_required(VERSION 3.24)
#    - Spécifie la version minimale de CMake requise
#    - ESP-IDF 6.1 s'appuie sur CMake 3.24+
#    - Garantit la compatibilité avec les fonctions CMake utilisées

# 2. set(PROJECT_NAME "nova_reptile_elevage")
#    - Définit une variable contenant le nom du projet
#    - Ce nom sera utilisé pour nommer l'exécutable final
#    - Convention : nom en minuscules avec underscores

# 3. include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#    - Inclut le système de build ESP-IDF
#    - $ENV{IDF_PATH} : variable d'environnement pointant vers ESP-IDF
#    - Ce fichier contient toute la logique de compilation ESP-IDF
#    - Gère automatiquement les composants, dépendances, flashage, etc.

# 4. project(${PROJECT_NAME})
#    - Déclare officiellement le projet CMake
#    - Active la recherche automatique des composants dans components/
#    - Configure les chemins d'inclusion et de liaison
#    - Crée la cible exécutable ${PROJECT_NAME}.elf

# 5. idf_build_set_property(COMPILE_DEFINITIONS ... APPEND)
#    - Ajoute des définitions de préprocesseur globales pour tous les composants
#    - LV_CONF_INCLUDE_SIMPLE=1 : indique à LVGL d'utiliser #include "lv_conf.h"
#    - LV_FONT_MONTSERRAT_30=1 : force la génération de la fonte personnalisée
#    - Ces définitions sont visibles par LVGL (Component Manager) et par l'application

# ===============================================
# FONCTIONNEMENT AVEC ESP-IDF
# ===============================================

# Lors de la compilation (idf.py build), CMake :
# 1. Lit ce CMakeLists.txt racine
# 2. Cherche automatiquement main/CMakeLists.txt (composant principal)
# 3. Scanne components/ pour les composants externes (LVGL)
# 4. Résout toutes les dépendances entre composants
# 5. Compile dans l'ordre correct
# 6. Génère nova_reptile_elevage.elf (firmware flashable)

# Structure détectée automatiquement :
# projet/
# ├── CMakeLists.txt          ← Ce fichier (configuration globale)
# ├── main/
# │   ├── CMakeLists.txt      ← Configuration du composant main
# │   └── *.c/*.h             ← Code source principal
# └── components/
#     └── lvgl_config/
#         ├── CMakeLists.txt  ← Configuration LVGL (lvgl_config)
#         └── lv_conf.h       ← Configuration LVGL
