/**
 * @file ui_sidebar.h
 * @brief Menu latéral de navigation
 * @author NovaReptileElevage Team
 */

#ifndef UI_SIDEBAR_H
#define UI_SIDEBAR_H

#include "lvgl.h"
#include "esp_err.h"
#include "ui_main.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise le menu latéral
 * @param parent Conteneur parent pour la sidebar
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_sidebar_init(lv_obj_t *parent);

/**
 * @brief Active un élément de menu spécifique
 * @param screen_type Type d'écran à activer
 */
void ui_sidebar_set_active_item(nova_screen_t screen_type);

/**
 * @brief Met à jour les compteurs/indicateurs du menu
 */
void ui_sidebar_update_indicators(void);

#ifdef __cplusplus
}
#endif

#endif // UI_SIDEBAR_H