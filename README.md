# NovaReptileElevage - Interface LVGL ESP32-S3

Interface utilisateur complète pour système d'élevage de reptiles utilisant LVGL 9.4.0 sur ESP32-S3 avec écran tactile 7 pouces.

## 🦎 Caractéristiques

- **Plateforme**: ESP32-S3 Touch LCD 7" (800x480)
- **Affichage**: Driver ST7262 avec support RGB565
- **Tactile**: Contrôleur GT911 multi-touch (5 points)
- **Interface**: LVGL 9.4.0 avec design moderne
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
components/
└── lvgl/
    ├── lv_conf.h         # Configuration LVGL partagée
    └── ...               # Bibliothèque LVGL
main/
├── main.c                # Point d'entrée principal
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

Le fichier `lv_conf.h` est placé dans `components/lvgl/` et, grâce à la définition `LV_CONF_INCLUDE_SIMPLE`, il est accessible à l'ensemble du projet.

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
// Interface SPI de configuration ST7262 (bus SPI3 dédié)
#define LCD_CMD_SPI_HOST   SPI3_HOST
#define LCD_CMD_MOSI_GPIO  11
#define LCD_CMD_SCLK_GPIO   6
#define LCD_CMD_CS_GPIO    12
#define LCD_CMD_DC_GPIO     4

// Tactile GT911 (I2C)
#define PIN_SDA        8
#define PIN_SCL        9
#define PIN_INT        4
#define TOUCH_PIN_RST  EXIO1 // via CH422G
```

Les broches MOSI et CS ont été déplacées respectivement sur GPIO11 et GPIO12 afin d'éliminer tout conflit avec les signaux RGB (PCLK en GPIO7 et DE en GPIO5).

#### Tableau de correspondance des broches

| Macro                | GPIO | Fonction                  | Fichier source                       |
|----------------------|------|---------------------------|--------------------------------------|
| `LCD_CMD_SPI_HOST`   | SPI3_HOST | Bus SPI commandes LCD | `components/st7262_rgb/st7262_rgb.c` |
| `LCD_CMD_SCLK_GPIO`  | 6    | SPI SCLK (cmd ST7262)     | `components/st7262_rgb/st7262_rgb.c` |
| `LCD_CMD_MOSI_GPIO`  | 11   | SPI MOSI (cmd ST7262)     | `components/st7262_rgb/st7262_rgb.c` |
| `LCD_CMD_CS_GPIO`    | 12   | SPI CS (cmd ST7262)       | `components/st7262_rgb/st7262_rgb.c` |
| `LCD_CMD_DC_GPIO`    | 4    | SPI D/C (cmd ST7262)      | `components/st7262_rgb/st7262_rgb.c` |
| `PIN_SDA`            | 8    | I2C SDA (GT911)           | `main/drivers/touch_driver.c`        |
| `PIN_SCL`            | 9    | I2C SCL (GT911)           | `main/drivers/touch_driver.c`        |
| `PIN_INT`            | 4    | Interruption tactile      | `main/drivers/touch_driver.c`        |
| `TOUCH_PIN_RST`      | CH422G EXIO1 | Reset contrôleur tactile  | `main/drivers/touch_driver.c`        |

### Partage du bus SPI
Le bus `SPI3_HOST` est dédié à l'interface de commande LCD et n'est pas partagé. Un éventuel lecteur **TF‑Card** doit être connecté sur `SPI2_HOST` ou sur un autre bus afin d'éviter toute contention.

### Paramètres SPI/I2C
- **SPI**: 40MHz, Mode 0, DMA activé
- **I2C**: 400kHz, Pull-ups activées
- **PWM**: Rétroéclairage contrôlable (5kHz, 8-bit)

## 🚀 Installation et compilation

### Prérequis
- ESP-IDF 5.4.2 ou supérieur
- Outil `idf.py` disponible dans le `PATH`
- LVGL 9.4.0 (inclus en composant)
- PSRAM activée, le frame buffer étant placé en PSRAM via `CONFIG_USE_PSRAM`
- Carte Waveshare ESP32-S3 Touch LCD 7"

#### Démarrage sans PSRAM

Si `CONFIG_USE_PSRAM` est désactivé, le frame buffer est alloué en SRAM interne; pour un écran 800x480, cela dépasse la capacité disponible et l'initialisation échoue. Lors de l'initialisation, le firmware vérifie la présence de la PSRAM via `esp_psram_init()` puis `esp_psram_get_chip_size()`. Si aucune PSRAM n'est détectée (taille nulle), l'initialisation est interrompue et l'erreur suivante est journalisée :

```
ESP_LOGE(TAG, "Aucune PSRAM détectée - initialisation annulée");
```

Dans ce cas, `nova_reptile_init()` renvoie `ESP_ERR_NO_MEM` avant toute configuration de LVGL. Activez la PSRAM dans `menuconfig` (**Component config → ESP PSRAM**) ou utilisez une carte dotée de SPIRAM pour exécuter l'application.

#### Dépannage PSRAM

- **Symptôme** : `PSRAM ID read error: 0x00ffffff`
- **Vérifications** :
  - Tension d'alimentation correcte
  - Configuration du mode ligne (1/2/4/8)
  - Straps GPIO0/GPIO46
  - Option `CONFIG_ESP32S3_SPIRAM_MODE_OCT` activée
- **Commandes de test** :
  ```bash
  idf.py monitor
  esp_idf_psram_test
  ```

### Libération en cas d'échec d'initialisation

`nova_reptile_init()` initialise séquentiellement l'affichage, le tactile et
l'interface. Si l'une de ces étapes échoue, les ressources déjà allouées sont
libérées dans l'ordre inverse suivant :

1. **Styles UI** via `ui_styles_deinit()`
2. **Driver tactile** via `touch_driver_deinit()`
3. **Driver d'affichage** via `display_driver_deinit()`
4. **Cœur LVGL** via `lv_deinit()`

Cette séquence garantit une désinitialisation propre sans fuite de ressources.

### Déinitialisation globale

Pour un arrêt contrôlé ou avant tout appel à `esp_restart()`,
`nova_reptile_deinit()` doit être invoquée. Cette fonction arrête et supprime
le timer `lvgl_tick_timer`, puis libère successivement les styles UI, le driver
tactile, le driver d'affichage et enfin le cœur LVGL.

### Étapes de compilation
```bash
# Clonage du projet
git clone <url_du_projet>
cd nova_reptile_elevage

# Configuration ESP-IDF
idf.py set-target esp32s3
idf.py menuconfig

# Compilation et flash
./scripts/build_flash.sh /dev/ttyUSB0 [--baud 921600] [--erase]
# --baud : définit la vitesse de flash
# --erase : efface la mémoire flash avant programmation
# ou manuellement
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

> **Note:** après toute modification de configuration, exécuter `idf.py defconfig` afin de régénérer `sdkconfig` à partir de `sdkconfig.defaults`.

### Configuration LVGL
Le fichier `components/lvgl/lv_conf.h` est configuré pour:
- Profondeur couleur 16-bit (RGB565)
- Buffer mémoire 64KB
- Widgets essentiels activés
- Polices Montserrat (12-28px)
- Support flex/grid layouts

## 🔄 Mises à jour OTA

Le projet prend en charge les mises à jour **OTA (Over-The-Air)** grâce à deux partitions OTA de 3 Mio chacune (`ota_0` et `ota_1`). Lorsqu'une nouvelle image est téléchargée, elle est stockée dans la partition inactive puis activée lors du redémarrage.

La table de partitions (`partitions.csv`) a été mise à jour en conséquence et la partition SPIFFS réduite à 1 Mio pour libérer l'espace requis.

Exemple minimal d'utilisation de l'API OTA d'ESP-IDF :

```c
esp_http_client_config_t config = {
    .url = "https://example.com/firmware.bin",
};
if (esp_https_ota(&config) == ESP_OK) {
    esp_restart();
}
```

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
