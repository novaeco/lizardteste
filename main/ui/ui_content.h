/**
 * @file ui_content.h
 * @brief Zone de contenu principal de l'interface
 * @author NovaReptileElevage Team
 */

#ifndef UI_CONTENT_H
#define UI_CONTENT_H

#include "lvgl.h"
#include "esp_err.h"
#include "ui_main.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise la zone de contenu principal
 * @param parent Conteneur parent pour le contenu
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_content_init(lv_obj_t *parent);

/**
 * @brief Charge un écran spécifique dans la zone de contenu
 * @param screen_type Type d'écran à charger
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_content_load_screen(nova_screen_t screen_type);

/**
 * @brief Met à jour les données temps réel du contenu
 */
void ui_content_update_realtime_data(void);

/**
 * @brief Obtient le conteneur de contenu actuel
 * @return lv_obj_t* Pointeur vers le conteneur
 */
lv_obj_t* ui_content_get_container(void);

#ifdef __cplusplus
}
#endif

#endif // UI_CONTENT_H