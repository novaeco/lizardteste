/**
 * @file ui_main.h
 * @brief Interface principale - Gestionnaire central de l'UI
 * @author NovaReptileElevage Team
 */

#ifndef UI_MAIN_H
#define UI_MAIN_H

#include "lvgl.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Dimensions de l'écran
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   480

// Dimensions des zones de l'interface
#define HEADER_HEIGHT   60
#define FOOTER_HEIGHT   40
#define SIDEBAR_WIDTH   180
#define CONTENT_WIDTH   (SCREEN_WIDTH - SIDEBAR_WIDTH)
#define CONTENT_HEIGHT  (SCREEN_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT)

// Structure principale de l'interface
typedef struct {
    lv_obj_t *main_screen;      // Écran principal
    lv_obj_t *header_container; // Conteneur header
    lv_obj_t *sidebar_container;// Conteneur sidebar
    lv_obj_t *content_container;// Conteneur contenu principal
    lv_obj_t *footer_container; // Conteneur footer
} nova_ui_t;

// Types d'écrans disponibles
typedef enum {
    SCREEN_DASHBOARD = 0,   // Tableau de bord principal
    SCREEN_REPTILES,        // Gestion des reptiles
    SCREEN_TERRARIUMS,      // Gestion des terrariums
    SCREEN_STATISTICS,      // Statistiques et graphiques
    SCREEN_ALERTS,          // Alertes et notifications
    SCREEN_SETTINGS,        // Paramètres système
    SCREEN_COUNT           // Nombre total d'écrans
} nova_screen_t;

/**
 * @brief Initialise l'interface utilisateur principale
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_main_init(void);

/**
 * @brief Obtient la structure UI principale
 * @return nova_ui_t* Pointeur vers la structure UI
 */
nova_ui_t* ui_main_get_instance(void);

/**
 * @brief Change l'écran actif
 * @param screen Type d'écran à afficher
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_main_set_screen(nova_screen_t screen);

/**
 * @brief Obtient l'écran actuellement actif
 * @return nova_screen_t Type d'écran actuel
 */
nova_screen_t ui_main_get_current_screen(void);

/**
 * @brief Met à jour les données temps réel de l'interface
 */
void ui_main_update_realtime_data(void);

/**
 * @brief Libère l'interface principale et ses ressources
 */
void ui_main_deinit(void);

/**
 * @brief Recharge les données configurables et reconstruit l'UI.
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_main_reload_data(void);

#ifdef __cplusplus
}
#endif

#endif // UI_MAIN_H
