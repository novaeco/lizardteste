# NovaReptileElevage - Interface LVGL ESP32-S3

Interface utilisateur complète pour système d'élevage de reptiles utilisant LVGL 9.2.2 sur ESP32-S3 avec écran tactile 7 pouces.

## 🦎 Caractéristiques

- **Plateforme**: ESP32-S3 Touch LCD 7" (800x480)
- **Affichage**: Driver ST7262 avec support RGB565
- **Tactile**: Contrôleur GT911 multi-touch (5 points)
- **Interface**: LVGL 9.2.2 avec design moderne
- **Architecture**: Modulaire et maintenable

## 📋 Structure de l'interface

```
┌─────────────────────────────────────────────────────────────┐
│                    BARRE DE TITRE (Header)                  │
│  [Logo] [NovaReptileElevage]           [Profil][Réglages]   │
├─────────────────┬───────────────────────────────────────────┤
│ MENU LATÉRAL    │           CONTENU PRINCIPAL               │
│                 │                                           │
│ 🏠 Accueil      │  • Tableau de bord                        │
│ 🦎 Reptiles     │  • Cartes d'information                   │
│ 🏺 Terrariums   │  • Graphiques et statistiques            │
│ 📊 Statistiques │  • Gestion des données                    │
│ ⚠️ Alertes      │  • Formulaires de configuration          │
│ ⚙️ Paramètres   │                                           │
├─────────────────┴───────────────────────────────────────────┤
│                    BARRE D'ÉTAT (Footer)                    │
│  Wi-Fi • Notifications • Heure • État système              │
└─────────────────────────────────────────────────────────────┘
```

## 🎨 Palette de couleurs

- **Fond principal**: #FAFAFA (blanc cassé), #F5F5F5 (gris clair)
- **Accents**: #4A90E2 (bleu), #7ED321 (vert), #F5A623 (orange)
- **Texte**: #2C3E50 (anthracite), #5D6D7E (moyen), #85929E (clair)
- **Interface**: Design moderne avec coins arrondis et ombres

## 🏗️ Architecture

### Structure des fichiers
```
main/
├── main.c                 # Point d'entrée principal
├── lv_conf.h             # Configuration LVGL
├── ui/                   # Interface utilisateur
│   ├── ui_main.c/.h      # Gestionnaire principal UI
│   ├── ui_header.c/.h    # Barre de titre
│   ├── ui_sidebar.c/.h   # Menu latéral
│   ├── ui_content.c/.h   # Zone de contenu
│   ├── ui_footer.c/.h    # Barre d'état
│   └── ui_styles.c/.h    # Styles personnalisés
└── drivers/              # Drivers matériels
    ├── display_driver.c/.h  # ST7262 (800x480)
    └── touch_driver.c/.h    # GT911 (tactile)
```

### Écrans disponibles
1. **Tableau de bord** - Vue d'ensemble du système
2. **Reptiles** - Gestion des animaux
3. **Terrariums** - Surveillance des habitats
4. **Statistiques** - Graphiques et analyses
5. **Alertes** - Notifications et problèmes
6. **Paramètres** - Configuration système

## 🔧 Configuration technique

### Broches ESP32-S3 (Waveshare 7")
```c
// Affichage ST7262
#define PIN_MOSI    11
#define PIN_MISO    13
#define PIN_CLK     12
#define PIN_CS      10
#define PIN_DC      14
#define PIN_RST     21
#define PIN_BCKL    2

// Tactile GT911
#define PIN_SDA     8
#define PIN_SCL     9
#define PIN_INT     18
#define PIN_RST     17
```

### Paramètres SPI/I2C
- **SPI**: 40MHz, Mode 0, DMA activé
- **I2C**: 400kHz, Pull-ups activées
- **PWM**: Rétroéclairage contrôlable (5kHz, 8-bit)

## 🚀 Installation et compilation

### Prérequis
- ESP-IDF 5.4.2 ou supérieur
- LVGL 9.2.2 (inclus en composant)
- Carte Waveshare ESP32-S3 Touch LCD 7"

### Étapes de compilation
```bash
# Clonage du projet
git clone <url_du_projet>
cd nova_reptile_elevage

# Configuration ESP-IDF
idf.py set-target esp32s3
idf.py menuconfig

# Compilation et flash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### Configuration LVGL
Le fichier `lv_conf.h` est configuré pour:
- Profondeur couleur 16-bit (RGB565)
- Buffer mémoire 64KB
- Widgets essentiels activés
- Polices Montserrat (12-28px)
- Support flex/grid layouts

## 📱 Fonctionnalités de l'interface

### Composants réutilisables
- **Cartes d'information** - Affichage de données avec titre/valeur
- **Boutons stylisés** - Primaire, secondaire, danger
- **Navigation fluide** - Transitions entre écrans
- **Indicateurs visuels** - États, alertes, notifications

### Gestion des événements
- **Tactile multi-point** - Support jusqu'à 5 points simultanés
- **Callbacks optimisés** - Gestion efficace des interactions
- **Feedback visuel** - États hover, pressed, active
- **Navigation intuitive** - Menu latéral et header

### Données temps réel
- **Mise à jour automatique** - Capteurs et états système
- **Indicateurs de connexion** - Wi-Fi, état réseau
- **Notifications** - Alertes et messages système
- **Horloge** - Date/heure en temps réel

## 🔍 Débogage et logs

### Niveaux de log disponibles
```c
ESP_LOGI(TAG, "Information générale");
ESP_LOGW(TAG, "Avertissement");
ESP_LOGE(TAG, "Erreur critique");
ESP_LOGD(TAG, "Debug (développement)");
```

### Monitoring
- Utilisation CPU/RAM en temps réel
- Température du processeur
- État des périphériques
- Performance de l'affichage

## 🎯 Extensibilité

### Ajout d'écrans
1. Créer la fonction `create_xxx_screen()` dans `ui_content.c`
2. Ajouter l'enum dans `ui_main.h`
3. Mettre à jour `menu_data[]` dans `ui_sidebar.c`
4. Implémenter le switch case dans `ui_content_load_screen()`

### Nouveaux styles
1. Définir les styles dans `ui_styles.c`
2. Ajouter les getters dans `ui_styles.h`
3. Utiliser avec `lv_obj_add_style()`

### Intégration capteurs
1. Créer les drivers dans `drivers/`
2. Ajouter les callbacks de mise à jour
3. Intégrer dans `ui_main_update_realtime_data()`

## 📚 Documentation LVGL

- [Documentation officielle LVGL](https://docs.lvgl.io)
- [Exemples et widgets](https://github.com/lvgl/lvgl/tree/master/examples)
- [Porting guide ESP32](https://docs.lvgl.io/master/porting/index.html)

## 🤝 Contribution

L'architecture modulaire facilite les contributions:
- Un fichier par composant UI
- Styles centralisés et réutilisables
- Drivers séparés pour chaque périphérique
- Documentation complète en français

## 📄 Licence

Projet NovaReptileElevage - Interface LVGL pour ESP32-S3
Compatible avec les licences ESP-IDF et LVGL.