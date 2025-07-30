/**
 * @file ui_footer.h
 * @brief Barre d'état (Footer) de l'interface
 * @author NovaReptileElevage Team
 */

#ifndef UI_FOOTER_H
#define UI_FOOTER_H

#include "lvgl.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise la barre d'état
 * @param parent Conteneur parent pour le footer
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_footer_init(lv_obj_t *parent);

/**
 * @brief Met à jour les informations de statut
 */
void ui_footer_update_status(void);

/**
 * @brief Met à jour l'indicateur Wi-Fi
 * @param connected État de la connexion
 * @param signal_strength Force du signal (0-100)
 */
void ui_footer_set_wifi_status(bool connected, int signal_strength);

/**
 * @brief Met à jour le nombre de notifications
 * @param count Nombre de notifications
 */
void ui_footer_set_notification_count(int count);

/**
 * @brief Met à jour l'affichage de l'heure
 * @param time_str Chaîne de l'heure complète
 */
void ui_footer_set_datetime(const char *time_str);

#ifdef __cplusplus
}
#endif

#endif // UI_FOOTER_H