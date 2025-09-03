cmake_minimum_required(VERSION 3.5)

# Configuration du projet NovaReptileElevage
set(PROJECT_NAME "nova_reptile_elevage")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(${PROJECT_NAME})

if(NOT DEFINED ENV{IDF_TARGET})
    set(ENV{IDF_TARGET} esp32s3)
    message("IDF_TARGET not set, defaulting to: $ENV{IDF_TARGET}")
endif()


# Configuration spécifique LVGL
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    LV_CONF_INCLUDE_SIMPLE=1
    LV_FONT_MONTSERRAT_30=1
)

# ===============================================
# EXPLICATION DÉTAILLÉE DU FICHIER CMAKE RACINE
# ===============================================

# 1. cmake_minimum_required(VERSION 3.5)
#    - Spécifie la version minimale de CMake requise
#    - ESP-IDF nécessite au minimum CMake 3.5
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

# 5. target_compile_definitions(${PROJECT_NAME}.elf PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
#    - Ajoute une définition de préprocesseur pour LVGL
#    - LV_CONF_INCLUDE_SIMPLE=1 : indique à LVGL d'utiliser #include "lv_conf.h"
#    - PRIVATE : cette définition n'est visible que dans ce projet
#    - Permet à LVGL de trouver automatiquement le fichier lv_conf.h

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
#     └── lvgl/
#         ├── CMakeLists.txt  ← Configuration du composant LVGL
#         └── lvgl/           ← Sources LVGL
