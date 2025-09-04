/**
 * @file touch_driver.h
 * @brief Driver pour contrôleur tactile GT911
 * @author NovaReptileElevage Team
 */

#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Configuration du GT911
#define TOUCH_MAX_POINTS    5    // Support multi-touch jusqu'à 5 points
#define TOUCH_WIDTH         800
#define TOUCH_HEIGHT        480

/**
 * @brief Initialise le driver tactile GT911
 * @return esp_err_t Code d'erreur ESP
 */
esp_err_t touch_driver_init(void);

/**
 * @brief Désactive le driver tactile
 */
void touch_driver_deinit(void);

/**
 * @brief Met le contrôleur GT911 en mode actif ou sommeil
 *        en manipulant les broches RST/INT.
 * @param enable true pour activer (réveil), false pour désactiver (sommeil)
 */
void touch_set_enable(bool enable);

/**
 * @brief Calibre l'écran tactile
 * @return esp_err_t Code d'erreur
 */
esp_err_t touch_calibrate(void);

#ifdef __cplusplus
}
#endif

#endif // TOUCH_DRIVER_H
