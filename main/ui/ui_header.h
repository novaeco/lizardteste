/**
 * @file ui_header.h
 * @brief Barre de titre (Header) de l'interface
 * @author NovaReptileElevage Team
 */

#ifndef UI_HEADER_H
#define UI_HEADER_H

#include "lvgl.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise la barre de titre
 * @param parent Conteneur parent pour le header
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_header_init(lv_obj_t *parent);

/**
 * @brief Met à jour le titre affiché
 * @param title Nouveau titre à afficher
 */
void ui_header_set_title(const char *title);

/**
 * @brief Met à jour l'indicateur de connexion
 * @param connected true si connecté, false sinon
 */
void ui_header_set_connection_status(bool connected);

/**
 * @brief Met à jour l'heure affichée
 * @param time_str Chaîne de caractères de l'heure (format HH:MM)
 */
void ui_header_set_time(const char *time_str);

/**
 * @brief Ouvre le menu profil
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_header_open_profile_menu(void);

/**
 * @brief Ouvre le panneau de réglages rapides
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_header_open_quick_settings(void);

#ifdef __cplusplus
}
#endif

#endif // UI_HEADER_H