#ifndef UI_DATA_H
#define UI_DATA_H

#include "lvgl.h"
#include "ui_main.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration d'un élément de menu latéral.
 */
typedef struct {
    const char *icon;      /**< Icône LVGL associée */
    const char *label;     /**< Libellé affiché */
    nova_screen_t screen;  /**< Écran associé */
} ui_menu_item_t;

/**
 * @brief Représentation d'une alerte simulée.
 */
typedef struct {
    const char *level;   /**< Niveau de l'alerte */
    const char *title;   /**< Titre de l'alerte */
    const char *details; /**< Détails supplémentaires */
} ui_alert_item_t;

extern ui_menu_item_t g_ui_menu_items[];
extern size_t g_ui_menu_items_count;

extern const char *g_ui_reptiles[];
extern size_t g_ui_reptiles_count;

extern ui_alert_item_t g_ui_alerts[];
extern size_t g_ui_alerts_count;

extern const char *g_ui_settings_sections[];
extern size_t g_ui_settings_sections_count;

void ui_data_load_defaults(void);
void ui_data_reload(void);

#ifdef __cplusplus
}
#endif

#endif // UI_DATA_H
