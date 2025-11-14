# Guide de Configuration ESP-IDF MenuConfig
## Projet NovaReptileElevage - Interface LVGL ESP32-S3

### Version du Guide
- **Projet** : NovaReptileElevage v1.0
- **Plateforme** : ESP32-S3 Touch LCD 7B (1024x600)
- **ESP-IDF** : 6.1
- **LVGL** : 9.4.0
- **Date** : Janvier 2025

---

## 🚀 Introduction

Ce guide vous accompagne dans la configuration complète de votre projet NovaReptileElevage via `idf.py menuconfig`. Suivez scrupuleusement chaque étape pour garantir le bon fonctionnement de votre interface LVGL sur ESP32-S3.

---

## 📋 Prérequis

Avant de commencer, vérifiez que :
- [ ] ESP-IDF 6.1 est correctement installé
- [ ] Les variables d'environnement ESP-IDF sont définies (`source ~/esp/esp-idf/export.sh`)
- [ ] LVGL 9.4.x est installé dans `components/lvgl/`
- [ ] Vous avez exécuté `idf.py set-target esp32s3`

---

## ⚙️ Configuration MenuConfig - Étapes Détaillées

### 1. Lancement de MenuConfig

```bash
cd /chemin/vers/votre/projet
idf.py menuconfig
```

Une interface de configuration basée sur un menu apparaît. Utilisez :
- **Flèches** : Navigation dans les menus
- **Entrée** : Sélectionner/Entrer dans un menu
- **Espace** : Cocher/Décocher une option
- **Échap** : Retour au menu précédent
- **S** : Sauvegarder et quitter
- **Q** : Quitter sans sauvegarder

---

### 2. Configuration SPI Flash (CRITIQUE)

**Chemin** : `Serial flasher config`

#### 2.1 Flash SPI Speed
```
Serial flasher config → Flash SPI speed
→ Sélectionner : 80 MHz
```
**Justification** : L'ESP32-S3 avec 16MB de flash supporte 80MHz pour des performances optimales.

#### 2.2 Flash SPI Mode  
```
Serial flasher config → Flash SPI mode
→ Sélectionner : DIO (Dual I/O)
```
**Justification** : Compatible avec la configuration par défaut Waveshare tout en conservant une vitesse élevée.

#### 2.3 Flash Size
```
Serial flasher config → Flash size
→ Sélectionner : 16 MB
```
**Justification** : Correspond à la taille flash de la carte Waveshare ESP32-S3 Touch LCD 7B.

---

### 3. Configuration Partition Table

**Chemin** : `Partition Table`

```
Partition Table → Partition Table
→ Sélectionner : Custom partition table CSV
→ Custom partition CSV file : partitions.csv
```

**Justification** : Permet d'optimiser l'allocation mémoire pour LVGL et les ressources graphiques.

---

### 4. Configuration Compiler

**Chemin** : `Compiler options`

#### 4.1 Optimization Level
```
Compiler options → Optimization Level
→ Sélectionner : Optimize for performance (-O2)
```

#### 4.2 Stack Protection
```
Compiler options → Stack smashing protection mode  
→ Sélectionner : Overall
```

**Justification** : Équilibre entre performance et sécurité pour une interface graphique fluide.

---

### 5. Configuration Component - ESP32-Specific

**Chemin** : `Component config → ESP32S3-Specific`

#### 5.1 CPU Frequency
```
ESP32S3-Specific → CPU frequency
→ Sélectionner : 240 MHz
```

#### 5.2 Cache Config
```
ESP32S3-Specific → Cache config
→ Instruction cache size : 32KB
→ Data cache size : 64KB
→ Instruction cache line size : 32 Bytes  
→ Data cache line size : 64 Bytes
```

**Justification** : Configuration optimale pour les opérations graphiques LVGL intensives.

---

### 6. Configuration FreeRTOS

**Chemin** : `Component config → FreeRTOS`

#### 6.1 Kernel
```
FreeRTOS → Kernel
→ configTICK_RATE_HZ : 1000
→ configMINIMAL_STACK_SIZE : 2048
```

#### 6.2 Port  
```
FreeRTOS → Port
→ configTIMER_TASK_STACK_DEPTH : 4096
→ ISR stack size : 2048
```

**Justification** : Stack suffisant pour LVGL et gestion temps réel optimisée.

---

### 7. Configuration SPI

**Chemin** : `Component config → SPI configuration`

```
SPI configuration
→ Enable SPI Master : [*] (activé)
→ Place SPI master ISR function into IRAM : [*] (activé)
→ SPI master ISR in IRAM : [*] (activé)
```

**Justification** : Performance maximale pour le driver ST7701 (écran 1024x600).

---

### 8. Configuration I2C

**Chemin** : `Component config → I2C configuration`

```
I2C configuration  
→ Enable I2C Master : [*] (activé)
→ I2C Master Timeout : 1000
```

**Justification** : Support du contrôleur tactile GT911 via I2C.

---

### 9. Configuration GPIO

**Chemin** : `Component config → GPIO configuration`

```
GPIO configuration
→ GPIO ISR in IRAM : [*] (activé)
```

**Justification** : Réactivité optimale pour les interruptions tactiles.

---

### 10. Configuration ESP Timer

**Chemin** : `Component config → High resolution timer (esp_timer)`

```
High resolution timer (esp_timer)
→ High-resolution timer task stack size : 4096
→ Enable esp_timer profiling features : [*] (activé)
```

**Justification** : Précision temporelle requise pour les animations LVGL.

---

### 11. Configuration PSRAM (CRITIQUE pour LVGL)

**Chemin** : `Component config → ESP PSRAM`

#### 11.1 Support for external, SPI-connected RAM
```
ESP PSRAM → Support for external, SPI-connected RAM : [*] (activé)
```

#### 11.2 Type of PSRAM chip in use
```
ESP PSRAM → Type of PSRAM chip in use
→ Sélectionner : Auto-detect
```

#### 11.3 SPI PSRAM clock and cs IO for ESP32S3
```
ESP PSRAM → SPI PSRAM clock and cs IO for ESP32S3
→ PSRAM CLK IO number : 30
→ PSRAM CS IO number : 26  
```

#### 11.4 PSRAM clock and cs IO for ESP32S3
```
ESP PSRAM → PSRAM clock and cs IO for ESP32S3
→ PSRAM speed : 80MHz clock
```

#### 11.5 PSRAM Allocation
```
ESP PSRAM → Initialize PSRAM in SPI mode : [*] (activé)
ESP PSRAM → Ignore PSRAM when not found : [ ] (désactivé)
```

**⚠️ CRITIQUE** : Cette configuration est essentielle pour les buffers LVGL sur un écran 1024x600.

---

### 12. Configuration Heap Memory

**Chemin** : `Component config → Heap memory debugging`

```
Heap memory debugging
→ Heap corruption detection : Light impact
→ Enable heap task tracking : [*] (activé)
```

**Justification** : Débogage mémoire pour détecter les fuites dans LVGL.

---

### 13. Configuration Log

**Chemin** : `Component config → Log output`

```
Log output
→ Default log verbosity : Info
→ Use ANSI terminal colors in log output : [*] (activé)
→ Log Timestamps : Microseconds
```

**Justification** : Débogage optimal pendant le développement.

---

### 14. Configuration WiFi (Optionnel)

**Chemin** : `Component config → Wi-Fi`

```
Wi-Fi → WiFi : [*] (activé si connexion réseau requise)
Wi-Fi → Station interface : [*] (activé)
Wi-Fi → SoftAP interface : [ ] (désactivé sauf besoin)
```

**Note** : Activez uniquement si votre application nécessite une connectivité réseau.

---

### 15. Configuration LWIP (Si WiFi activé)

**Chemin** : `Component config → LWIP`

```
LWIP → Max number of open sockets : 10
LWIP → TCP SND buffer : 16384
LWIP → TCP WND buffer : 32768
```

---

### 16. Configuration Power Management

**Chemin** : `Component config → Power Management`

```
Power Management → Support for power management : [*] (activé)
Power Management → Enable dynamic frequency scaling (DFS) : [*] (activé)
```

**Justification** : Économie d'énergie lorsque l'interface n'est pas active.

---

## 🔧 Configuration Spécifique LVGL

### 17. Vérification lv_conf.h

Le fichier `components/lvgl/lv_conf.h` est partagé dans tout le projet via `LV_CONF_INCLUDE_SIMPLE`.
Assurez-vous que `components/lvgl/lv_conf.h` contient :

```c
// Optimisations ESP32-S3
#define LV_COLOR_DEPTH 16
#define LV_MEM_SIZE (64U * 1024U)  // 64KB pour les buffers
#define LV_DISP_DEF_REFR_PERIOD 16  // 60 FPS
#define LV_INDEV_DEF_READ_PERIOD 16 // Tactile réactif

// Widgets activés pour NovaReptileElevage
#define LV_USE_BTN 1
#define LV_USE_LABEL 1
#define LV_USE_CHART 1
#define LV_USE_FLEX 1
#define LV_USE_GRID 1
```

---

## 💾 Sauvegarde et Test

### 18. Finalisation

1. **Sauvegarde de la configuration** :
   - Appuyez sur `S` dans menuconfig
   - Confirmez la sauvegarde

2. **Test de compilation** :
   ```bash
   idf.py build
   ```

3. **Vérification des warnings** :
   - Aucun warning critique ne doit apparaître
   - Les warnings LVGL sont normaux

---

## 🚨 Dépannage Courant

### Erreur de Mémoire PSRAM
**Symptômes** : `PSRAM not found` ou `Guru Meditation Error`
**Solution** : Vérifiez les broches PSRAM (CLK=30, CS=26) dans la configuration.

### Écran Noir
**Symptômes** : Pas d'affichage malgré la compilation réussie
**Solution** : Vérifiez la configuration SPI et les broches dans `display_driver.c`.

### Interface Lente
**Symptômes** : Interface qui rame, animations saccadées  
**Solution** : Activez PSRAM et vérifiez que CPU=240MHz.

### Problème Tactile
**Symptômes** : Pas de réponse au toucher
**Solution** : Vérifiez I2C activé et broches GT911 (SDA=8, SCL=9).

---

## 📚 Fichiers de Configuration Générés

Après `idf.py menuconfig`, ces fichiers sont créés/modifiés :
- `sdkconfig` : Configuration principale (ne pas modifier manuellement)
- `sdkconfig.old` : Sauvegarde automatique
- `build/config/` : Fichiers de configuration compilés

---

## ✅ Check-list Finale

Avant de compiler (`idf.py build`) :

- [ ] ESP32-S3 Target défini (`idf.py set-target esp32s3`)
- [ ] PSRAM activé et configuré (CLK=30, CS=26)
- [ ] SPI et I2C activés 
- [ ] CPU à 240MHz
- [ ] Flash à 80MHz en mode DIO
- [ ] Optimization -Os
- [ ] LVGL 9.4.x installé dans components/
- [ ] Configuration sauvegardée dans menuconfig

---

## 🏁 Conclusion

Cette configuration optimise votre ESP32-S3 pour l'interface LVGL NovaReptileElevage. Respectez chaque étape pour garantir :
- **Performance graphique optimale** (60 FPS)
- **Réactivité tactile** parfaite
- **Stabilité mémoire** avec PSRAM
- **Débogage** facilité

En cas de problème, consultez les logs ESP-IDF et vérifiez que tous les paramètres correspondent exactement à ce guide.

**Bonne programmation avec NovaReptileElevage ! 🦎**
